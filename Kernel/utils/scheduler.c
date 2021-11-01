#include "../include/pipes.h"
#include "../include/naiveConsole.h"
#include "../interruptions/interrupts.h"
#include "../interruptions/time.h"

#define QUANTUM 1
#define PRIORITY_QUANTUM 5
#define MAX_WAITING_KEYBOARD 10
#define DEFAULT_PROGRAM_SIZE 4096

typedef struct ListNode {
  pcb process;
  struct ListNode *next; 
} ListNode;

typedef struct WaitingNode {
  pcb *process;
  struct WaitingNode *next; 
} WaitingNode;

typedef struct WaitingKeyboardList {
  WaitingNode *current;
  WaitingNode *tail;
  uint8_t size;
} WaitingKeyboardList;

typedef struct Scheduler {
  uint32_t quantum;
  uint32_t priorityQuantum;
  uint8_t fgTaken;
  ListNode *currentProcess;
  ListNode *start;  // lista ordenada por prioridades, los mas prioritarios primero
} Scheduler;

Scheduler *scheduler;
uint8_t firstProcess;
uint32_t pid;
ListNode *dummy;
WaitingKeyboardList *waitingKeyboardList;

static void dummyProcess() {
  while(1) {
    if (ticks_elapsed() % 9 == 0)
      displayCursor();
    _hlt();
  }
}

void initScheduler() {
  scheduler = (Scheduler *) alloc(sizeof(Scheduler));
  scheduler->currentProcess = NULL;
  scheduler->quantum = QUANTUM-1;
  scheduler->priorityQuantum = PRIORITY_QUANTUM;
  scheduler->start = NULL;
  scheduler->fgTaken = 0;
  pid = 1;
  firstProcess = 1;

  waitingKeyboardList = (WaitingKeyboardList *) alloc(sizeof(WaitingKeyboardList));
  waitingKeyboardList->current = (WaitingNode *) alloc(sizeof(WaitingNode));
  waitingKeyboardList->current->process = NULL;
  waitingKeyboardList->current->next = NULL;
  waitingKeyboardList->tail = waitingKeyboardList->current;
  waitingKeyboardList->size = 0;
  WaitingNode *aux = waitingKeyboardList->current;
  for (int i = 0; i < MAX_WAITING_KEYBOARD; i++) {
    WaitingNode *newNode = (WaitingNode *) alloc(sizeof(WaitingNode));
    newNode->process = NULL;
    newNode->next = NULL;
    aux->next = newNode;
    aux = aux->next;
  }
  aux->next = waitingKeyboardList->current;

  uint64_t dummyMemory = (uint64_t) alloc(2048);
  
	uint64_t sp = initProcess(dummyMemory + 2048, (uint64_t)&dummyProcess, 0, NULL);
  dummy = (ListNode *) alloc(sizeof(ListNode));
  dummy->process.pid = 0;
  dummy->process.sp = sp;
  dummy->process.processMemory = dummyMemory;
  dummy->process.pstate = 1;
  dummy->process.priority = 1;
  dummy->next = scheduler->start;
}

static char* copyString(char* destination, const char* source) {
  if (destination == NULL)
    return NULL;

  char *ptr = destination;

  while (*source != '\0') {
    *destination = *source;
    destination++;
    source++;
  }

  *destination = '\0';
  return ptr;
}

static ListNode *loadProcess(ListNode * node, uint32_t pid, uint8_t priority, int argc, char args[6][21], fdPipe *customStdin, fdPipe *customStdout, uint64_t ip) {

  if (node == NULL) {
    ListNode *newNode = (ListNode *) alloc(sizeof(ListNode));
    newNode->process.pid = pid;
    newNode->process.pstate = 1;
    newNode->process.priority = priority;
    newNode->process.auxPriority = priority;
    for (int i = 0; i < argc; i++)
      copyString(newNode->process.args[i], args[i]);
    uint64_t processMemory = (uint64_t) alloc(DEFAULT_PROGRAM_SIZE);
	  uint64_t sp = initProcess(processMemory + DEFAULT_PROGRAM_SIZE, ip, argc, newNode->process.args);
    newNode->process.sp = sp;
    newNode->process.bp = processMemory + DEFAULT_PROGRAM_SIZE - 1;
    newNode->process.processMemory = processMemory;
    newNode->process.customStdin = customStdin;
    newNode->process.customStdout = customStdout;
    return newNode;
  }

  if (node->next != NULL && priority >= node->next->process.priority) {
    node->next = loadProcess(node->next, pid, priority, argc, args, customStdin, customStdout, ip);
    return node;
  }

  ListNode *newNode = (ListNode *) alloc(sizeof(ListNode));

  newNode->next = node->next;
  node->next = newNode;
  
  newNode->process.pid = pid;
  newNode->process.pstate = 1; // por defecto pstate = 1 = ready
  newNode->process.priority = priority;
  newNode->process.auxPriority = priority;
  for (int i = 0; i < argc; i++)
    copyString(newNode->process.args[i], args[i]);
  uint64_t processMemory = (uint64_t) alloc(DEFAULT_PROGRAM_SIZE);
  uint64_t sp = initProcess(processMemory + DEFAULT_PROGRAM_SIZE, ip, argc, newNode->process.args);
  newNode->process.sp = sp;
  newNode->process.bp = processMemory + DEFAULT_PROGRAM_SIZE - 1;
  newNode->process.processMemory = processMemory;
  newNode->process.customStdin = customStdin;
  newNode->process.customStdout = customStdout;

  return node;
}

int createProcess(uint64_t ip,uint8_t priority, uint64_t argc, char argv[6][21], fdPipe *customStdin, fdPipe *customStdout) {
  if(priority == 1 && pid > 1)
    scheduler->fgTaken = 1;

  scheduler->start = loadProcess(scheduler->start, pid++, priority, argc, argv, customStdin, customStdout, ip);
  return pid-1;
}

int createProcessWrapper(uint64_t ip, uint8_t priority, uint64_t argc, char *argv, fdPipe *customStdin, fdPipe *customStdout) {
  int i = 0, j = 0;
  char args[6][21];
  while(i < argc) {
    int k = 0;
    while(argv[j]) {
      args[i][k] = argv[j];
      j++;
      k++;
    }
    args[i][k] = 0;
    j++;
    i++;
  }

  return createProcess(ip, priority, argc, args, customStdin, customStdout);
}

uint64_t switchProcess(uint64_t sp) {

  // Para el primer proceso no actualizamos el sp, usamos el hardcodeado
  if (firstProcess) {
    firstProcess = 0;
    scheduler->currentProcess = scheduler->start;
    return scheduler->start->process.sp;
  }
  if(scheduler->quantum > 0) {
    scheduler->quantum--;
    return 0;
  }
  scheduler->quantum = QUANTUM-1;

  // Aumento la prioridad de todos
  if(scheduler->priorityQuantum == 0) {
    ListNode * auxNodeIter = scheduler->start;
    while(auxNodeIter != NULL) {
      if(auxNodeIter->process.auxPriority > 1 && auxNodeIter->process.pid != scheduler->currentProcess->process.pid && auxNodeIter->process.pstate == 1)  
        auxNodeIter->process.auxPriority--;
      auxNodeIter = auxNodeIter->next;
    }
    scheduler->priorityQuantum = PRIORITY_QUANTUM+1;
  }
  scheduler->priorityQuantum--;


  // guardar el sp del proceso actual en su PCB
  scheduler->currentProcess->process.sp = sp;

  ListNode *nextProcess = scheduler->currentProcess->next;
  ListNode *auxNextProcess = nextProcess;
  while(auxNextProcess != NULL) {
    if(nextProcess->process.pstate != 1)
      nextProcess = auxNextProcess;
    if(auxNextProcess->process.auxPriority < nextProcess->process.auxPriority && auxNextProcess->process.pstate == 1)
      nextProcess = auxNextProcess;
    auxNextProcess = auxNextProcess->next;
  }

  ListNode *firstMinReadyProcess = scheduler->start;
  ListNode *auxFirstMinReadyProcess = firstMinReadyProcess;
  while(auxFirstMinReadyProcess != NULL && auxFirstMinReadyProcess->process.pid != scheduler->currentProcess->process.pid) {
    if (auxFirstMinReadyProcess->process.pid == 1 && scheduler->fgTaken) {
      if (firstMinReadyProcess->process.pid == 1)
        firstMinReadyProcess = firstMinReadyProcess->next;
      auxFirstMinReadyProcess = auxFirstMinReadyProcess->next;
      continue;
    }

    if(firstMinReadyProcess->process.pstate != 1 && (!scheduler->fgTaken || auxFirstMinReadyProcess->process.pid != 1)) {
      firstMinReadyProcess = auxFirstMinReadyProcess;
    }

    
    if(auxFirstMinReadyProcess->process.auxPriority < firstMinReadyProcess->process.auxPriority && firstMinReadyProcess->process.pstate == 1)
      firstMinReadyProcess = auxFirstMinReadyProcess;
    auxFirstMinReadyProcess = auxFirstMinReadyProcess->next;
  }

  if(firstMinReadyProcess->process.pstate != 1 && scheduler->currentProcess->process.pstate == 1 && scheduler->currentProcess->process.pid != 0)
    firstMinReadyProcess = scheduler->currentProcess;

  if(scheduler->fgTaken && firstMinReadyProcess->process.pid == 1) 
    firstMinReadyProcess = NULL;

  if((nextProcess == NULL || nextProcess->process.pstate != 1) && (firstMinReadyProcess == NULL || firstMinReadyProcess->process.pstate != 1)) {
    scheduler->currentProcess = dummy;
    return scheduler->currentProcess->process.sp;
  } else if(nextProcess == NULL || nextProcess->process.pstate != 1)
    scheduler->currentProcess = firstMinReadyProcess;
  else if(firstMinReadyProcess == NULL || firstMinReadyProcess->process.pstate != 1)
    scheduler->currentProcess = nextProcess;
  else 
    if(firstMinReadyProcess->process.auxPriority < nextProcess->process.auxPriority)
      scheduler->currentProcess = firstMinReadyProcess;
    else 
      scheduler->currentProcess = nextProcess;

  scheduler->currentProcess->process.auxPriority = scheduler->currentProcess->process.priority;
  return scheduler->currentProcess->process.sp;
}

static ListNode * deleteProcess(ListNode *node, uint32_t pid) {
  if (node == NULL)
    return node;

  if (node->process.pid == pid) {
    node->process.pstate = 2;
    ListNode *aux = node->next;
    deleteProcessFromSemaphores(pid);
    deleteProcessFromPipes(pid);
    free((void *)node->process.processMemory);
    free((void *)node);
    return aux;
  }

  node->next = deleteProcess(node->next, pid);
  return node;
}

void exitCurrentProcess() {
  if(scheduler->currentProcess->process.priority == 1)
    scheduler->fgTaken = 0;

  scheduler->start = deleteProcess(scheduler->start, scheduler->currentProcess->process.pid);
}

void killPid(uint32_t pid) {
  if(pid > 1)
    scheduler->start = deleteProcess(scheduler->start, pid);
}

void printProcessList() {
  ncPrint("Name    PID    Priority     SP       BP     Type        State\n");
  ListNode *aux = scheduler->start;
  while(aux != NULL) {
    ncPrint(aux->process.args[0]);
    ncPrint("     ");
    ncPrintDec(aux->process.pid);
    ncPrint("     ");
    ncPrintDec(aux->process.priority);
    ncPrint("            ");
    ncPrintHex(aux->process.sp);
    ncPrint("     ");
    ncPrintHex(aux->process.bp);
    ncPrint("     ");
    ncPrint(aux->process.priority == 1 ? "foreground" : "background");
    ncPrint("     ");
    ncPrint(aux->process.pstate ? "Ready" : "Blocked");
    ncNewline();
    aux = aux->next;
  }
}

static pcb *getPCB(ListNode *node, uint32_t pid) {
  if(node == NULL)
    return NULL;

  if(node->process.pid == pid)
    return &node->process;

  return getPCB(node->next, pid);
}  

void changeProcessPriority(uint32_t pid, uint8_t newPriority) {
  if (pid <= 1 || newPriority <= 1)
    return;
  
  pcb *pidPCB = getPCB(scheduler->start, pid);
  if (pidPCB == NULL)
    return;

  pidPCB->priority = newPriority;
  pidPCB->auxPriority = newPriority;

  ListNode *headPID = scheduler->start;
  while (headPID->next->process.pid != pid)
    headPID = headPID->next;

  ListNode *process = headPID->next;
  headPID->next = process->next;

  headPID = scheduler->start;
  while (headPID->next != NULL && newPriority >= headPID->next->process.priority)
    headPID = headPID->next;

  ListNode *aux = headPID->next;
  headPID->next = process;
  process->next = aux;
}

void changeProcessState(uint32_t pid) {
  if (pid <= 1)
    return;

  pcb *pidPCB = getPCB(scheduler->start, pid);
  if (pidPCB == NULL)
    return;

  if (pidPCB->pstate == 1) {
    pidPCB->pstate = 0;
    if(pid == scheduler->currentProcess->process.pid)
      runScheduler();
  } else if (pidPCB -> pstate == 0)
    pidPCB->pstate = 1;
}

void waitForKeyboard() {
  scheduler->currentProcess->process.pstate = 0;
  waitingKeyboardList->tail->process = &scheduler->currentProcess->process;
  waitingKeyboardList->tail = waitingKeyboardList->tail->next;
  waitingKeyboardList->size++;
  runScheduler();
}

void awakeKeyboardQueue() {
  if (waitingKeyboardList->size == 0)
    return;
  waitingKeyboardList->size--;
  waitingKeyboardList->current->process->pstate = 1;
  waitingKeyboardList->current = waitingKeyboardList->current->next;
}

pcb *blockCurrentProcess() {
  scheduler->currentProcess->process.pstate = 0;
  return &scheduler->currentProcess->process;
}

fdPipe *getCurrentStdin() {
  return scheduler->currentProcess->process.customStdin;
}

fdPipe *getCurrentStdout() {
  return scheduler->currentProcess->process.customStdout;
}

uint32_t getCurrentPid() {
  return scheduler->currentProcess->process.pid;
}

pcb *getCurrentProcess() {
  return &scheduler->currentProcess->process;
}