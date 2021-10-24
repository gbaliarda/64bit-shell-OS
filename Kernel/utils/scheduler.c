#include "../include/scheduler.h"
#include "../include/naiveConsole.h"
#include "../include/mm.h"
#include "../include/lib.h"
#include "../interruptions/interrupts.h"

#define QUANTUM 1
#define PRIORITY_QUANTUM 5
#define MAX_PROCESS_AMOUNT 10
#define NULL ((void*) 0)

typedef struct pcb {
  char *name;
  uint8_t type;
  uint32_t pid;
  uint8_t pstate;
  uint8_t priority;
  uint8_t auxPriority;
  uint64_t sp;
  uint64_t bp;
  uint64_t processMemory;
} pcb;

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
    _hlt();
  }
}

void initScheduler() {
  scheduler = (Scheduler *) alloc(sizeof(Scheduler) + sizeof(ListNode)* MAX_PROCESS_AMOUNT);
  scheduler->currentProcess = NULL;
  scheduler->quantum = QUANTUM-1;
  scheduler->priorityQuantum = PRIORITY_QUANTUM;
  scheduler->start = NULL;
  pid = 1;
  firstProcess = 1;

  waitingKeyboardList = (WaitingKeyboardList *) alloc(sizeof(WaitingKeyboardList));
  waitingKeyboardList->current = (WaitingNode *) alloc(sizeof(WaitingNode));
  waitingKeyboardList->current->process = NULL;
  waitingKeyboardList->current->next = NULL;
  waitingKeyboardList->tail = waitingKeyboardList->current;
  waitingKeyboardList->size = 0;
  WaitingNode *aux = waitingKeyboardList->current;
  for (int i = 0; i < MAX_PROCESS_AMOUNT; i++) {
    WaitingNode *newNode = (WaitingNode *) alloc(sizeof(WaitingNode));
    newNode->process = NULL;
    newNode->next = NULL;
    aux->next = newNode;
    aux = aux->next;
  }
  aux->next = waitingKeyboardList->current;

  uint64_t dummyMemory = (uint64_t) alloc(500);
  
	uint64_t sp = initProcess(dummyMemory + 500, (uint64_t)&dummyProcess, 0, NULL);
  dummy = (ListNode *) alloc(sizeof(ListNode));
  dummy->process.pid = 0;
  dummy->process.sp = sp;
  dummy->process.processMemory = dummyMemory;
  dummy->process.pstate = 1;
  dummy->process.priority = 1;
  dummy->process.name = "Dummy";
  dummy->next = scheduler->start;
}

static ListNode *loadProcess(ListNode * node, uint32_t pid, uint8_t priority, uint64_t sp, uint64_t processMemory, char *name) {

  if (node == NULL) {
    ListNode *newNode = (ListNode *) ((uint64_t)scheduler + sizeof(Scheduler));
    newNode->process.pid = pid;
    newNode->process.pstate = 1;
    newNode->process.priority = priority;
    newNode->process.auxPriority = priority;
    newNode->process.sp = sp;
    newNode->process.bp = sp;
    newNode->process.processMemory = processMemory;
    newNode->process.type = 1;
    newNode->process.name = name;
    return newNode;
  }

  if (node->next != NULL && priority >= node->next->process.priority) {
    node->next = loadProcess(node->next, pid, priority, sp, processMemory, name);
    return node;
  }

  ListNode *newNode = (ListNode *) ((uint64_t)node + sizeof(ListNode));

  newNode->next = node->next;
  node->next = newNode;
  
  newNode->process.pid = pid;
  newNode->process.pstate = 1; // por defecto pstate = 1 = ready
  newNode->process.priority = priority;
  newNode->process.auxPriority = priority;
  newNode->process.sp = sp;
  newNode->process.bp = sp;
  newNode->process.processMemory = processMemory;
  newNode->process.type = 1;
  newNode->process.name = name;
  return node;
}

void createProcess(uint64_t ip, uint32_t size, uint8_t priority, uint64_t argc, char ** argv) {
  uint64_t processMemory = (uint64_t) alloc(size);
	uint64_t sp = initProcess(processMemory + size, ip, argc, argv);
  scheduler->start = loadProcess(scheduler->start, pid++, priority, sp, processMemory, argv[0]);
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
    if(firstMinReadyProcess->process.pstate != 1)
      firstMinReadyProcess = auxFirstMinReadyProcess;

    if(auxFirstMinReadyProcess->process.auxPriority < firstMinReadyProcess->process.auxPriority && firstMinReadyProcess->process.pstate == 1)
      firstMinReadyProcess = auxFirstMinReadyProcess;
    auxFirstMinReadyProcess = auxFirstMinReadyProcess->next;
  }

  if(firstMinReadyProcess->process.pstate != 1 && scheduler->currentProcess->process.pstate == 1 && scheduler->currentProcess->process.pid != 0)
    firstMinReadyProcess = scheduler->currentProcess;

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
    free((void *)node->process.processMemory);
    return node->next;
  }

  node->next = deleteProcess(node->next, pid);
  return node;
}

void exitCurrentProcess() {
  scheduler->start = deleteProcess(scheduler->start, scheduler->currentProcess->process.pid);
}

void killPid(uint32_t pid) {
  scheduler->start = deleteProcess(scheduler->start, pid);
}

void printProcessList() {
  ncPrint("Nombre    PID    Prioridad     SP       BP     Tipo        Estado\n");
  ListNode *aux = scheduler->start;
  while(aux != NULL) {
    ncPrint(aux->process.name);
    ncPrint("     ");
    ncPrintDec(aux->process.pid);
    ncPrint("     ");
    ncPrintDec(aux->process.priority);
    ncPrint("            ");
    ncPrintHex(aux->process.sp);
    ncPrint("     ");
    ncPrintHex(aux->process.bp);
    ncPrint("     ");
    ncPrint(aux->process.type == 1 ? "foreground" : "background");
    ncPrint("     ");
    ncPrint(aux->process.pstate ? "Ready" : "Bloqueado");
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
  // pcb *pidPCB = getPCB(scheduler->start, pid);
  // scheduler->start = deleteProcess(scheduler->start, pid);
  // createProcess
}

void changeProcessState(uint32_t pid) {
  //
}

void waitForKeyboard() {
  scheduler->currentProcess->process.pstate = 0;
  waitingKeyboardList->tail->process = &scheduler->currentProcess->process;
  waitingKeyboardList->tail = waitingKeyboardList->tail->next;
  waitingKeyboardList->size++;
  _sti();
  runScheduler();
}

void awakeKeyboardQueue() {
  if(waitingKeyboardList->size == 0)
    return;
  waitingKeyboardList->size--;
  waitingKeyboardList->current->process->pstate = 1;
  waitingKeyboardList->current = waitingKeyboardList->current->next;
}