#include "../include/scheduler.h"
#include "../include/naiveConsole.h"
#include "../include/mm.h"
#include "../include/lib.h"

#define QUANTUM 1
#define PRIORITY_QUANTUM 5
#define MAX_PROCESS_AMOUNT 10
#define NULL ((void*) 0)

typedef struct pcb {
  uint32_t pid;
  uint8_t pstate;
  uint8_t priority;
  uint8_t auxPriority;
  uint64_t sp;
  uint64_t processMemory;
} pcb;

typedef struct ListNode {
  pcb process;
  struct ListNode *next; 
} ListNode;

typedef struct Scheduler {
  uint32_t quantum;
  uint32_t priorityQuantum;
  ListNode *currentProcess;
  ListNode *start;  // lista ordenada por prioridades, los mas prioritarios primero
} Scheduler;

Scheduler *scheduler;
uint8_t firstProcess;
uint32_t pid;

void initScheduler() {
  scheduler = (Scheduler *) alloc(sizeof(Scheduler) + sizeof(ListNode)* MAX_PROCESS_AMOUNT);
  scheduler->currentProcess = NULL;
  scheduler->quantum = QUANTUM-1;
  scheduler->priorityQuantum = PRIORITY_QUANTUM;
  scheduler->start = NULL;
  pid = 0;
  firstProcess = 1;
}

static ListNode *loadProcess(ListNode * node, uint32_t pid, uint8_t priority, uint64_t sp, uint64_t processMemory) {

  if (node == NULL) {
    ListNode *newNode = (ListNode *) ((uint64_t)scheduler + sizeof(Scheduler));
    newNode->process.pid = pid;
    newNode->process.pstate = 1;
    newNode->process.priority = priority;
    newNode->process.auxPriority = priority;
    newNode->process.sp = sp;
    newNode->process.processMemory = processMemory;
    return newNode;
  }

  if (node->next != NULL && priority >= node->next->process.priority) {
    node->next = loadProcess(node->next, pid, priority, sp, processMemory);
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
  newNode->process.processMemory = processMemory;
  return node;
}

void createProcess(uint64_t ip, uint32_t size, uint8_t priority) {
  uint64_t processMemory = (uint64_t) alloc(size);
	uint64_t sp = initProcess(processMemory + size, ip);
  scheduler->start = loadProcess(scheduler->start, pid++, priority, sp, processMemory);
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
      if(auxNodeIter->process.auxPriority > 1 && auxNodeIter->process.pid != scheduler->currentProcess->process.pid)  
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

    if(auxNextProcess->process.auxPriority < nextProcess->process.auxPriority && auxNextProcess->process.pstate)
      nextProcess = auxNextProcess;
    auxNextProcess = auxNextProcess->next;
  }

  ListNode *firstMinReadyProcess = scheduler->start;
  ListNode *auxFirstMinReadyProcess = firstMinReadyProcess;
  while(auxFirstMinReadyProcess != NULL && auxFirstMinReadyProcess->process.pid != scheduler->currentProcess->process.pid) {
    if(firstMinReadyProcess->process.pstate != 1)
      firstMinReadyProcess = auxFirstMinReadyProcess;

    if(auxFirstMinReadyProcess->process.auxPriority < firstMinReadyProcess->process.auxPriority && firstMinReadyProcess->process.pstate)
      firstMinReadyProcess = auxFirstMinReadyProcess;
    auxFirstMinReadyProcess = auxFirstMinReadyProcess->next;
  }

  if(firstMinReadyProcess->process.pstate != 1 && scheduler->currentProcess->process.pstate)
    firstMinReadyProcess = scheduler->currentProcess;

  if((nextProcess == NULL && firstMinReadyProcess == NULL) || (nextProcess->process.pstate != 1 && firstMinReadyProcess->process.pstate != 1) ) {
    // dummy
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
    free((void *)node->process.processMemory);
    return node->next;
  }

  node->next = deleteProcess(node->next, pid);
  return node;
}

void exitCurrentProcess() {
  scheduler->start = deleteProcess(scheduler->start, scheduler->currentProcess->process.pid);
}
