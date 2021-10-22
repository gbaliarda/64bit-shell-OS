#include "../include/scheduler.h"
#include "../include/naiveConsole.h"
#include "../include/mm.h"
#include "../include/lib.h"

#define QUANTUM 1
#define PRIORITY_QUANTUM 5
#define MAX_CANTIDAD_PROCESOS 10
#define NULL ((void*) 0)

typedef struct pcb {
  uint32_t pid;
  uint8_t pstate;
  uint8_t priority;
  uint8_t auxPriority;
  uint64_t sp;
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
  scheduler = (Scheduler *) alloc(sizeof(Scheduler) + sizeof(ListNode)*MAX_CANTIDAD_PROCESOS);
  scheduler->currentProcess = NULL;
  scheduler->quantum = QUANTUM;
  scheduler->priorityQuantum = PRIORITY_QUANTUM;
  scheduler->start = NULL;
  pid = 0;
  firstProcess = 1;
}

static ListNode *loadProcess(ListNode * node, uint32_t pid, uint8_t priority, uint64_t sp) {

  if (node == NULL) {
    ListNode *newNode = (ListNode *) ((uint64_t)scheduler + sizeof(Scheduler));
    newNode->process.pid = pid;
    newNode->process.pstate = 1;
    newNode->process.priority = priority;
    newNode->process.auxPriority = priority;
    newNode->process.sp = sp;
    return newNode;
  }

  if (node->next != NULL && priority >= node->next->process.priority) {
    node->next = loadProcess(node->next, pid, priority, sp);
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
  return node;
}

void createProcess(uint64_t ip, uint32_t size, uint8_t priority) {
  uint64_t process = (uint64_t) alloc(size);
	uint64_t sp = initProcess(process + size, ip);
  scheduler->start = loadProcess(scheduler->start, pid++, priority, sp);
}

uint64_t switchProcessOld(uint64_t sp) {
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
  scheduler->quantum = QUANTUM;

  // guardar el sp del proceso actual en su PCB
  scheduler->currentProcess->process.sp = sp;
  
  int nextSamePriority = 0;
  // avanzas al siguiente del current si tiene la misma prioridad
  if (scheduler->currentProcess->next != NULL && scheduler->currentProcess->next->process.priority == scheduler->currentProcess->process.priority) {
    scheduler->currentProcess = scheduler->currentProcess->next;
    nextSamePriority = 1;
  }

  // si no hay siguiente con misma prioridad
  ListNode * nodeIter = scheduler->start;
  while(nodeIter != NULL && nodeIter->process.pstate != 1)
    nodeIter = nodeIter->next;

  // deberia haber un dummy para que siempre haya un proceso ready, por lo que esto no podria pasar
  if(nodeIter == NULL)
    return 0;

  if (nodeIter->process.priority <= scheduler->currentProcess->process.priority) {
    if (nodeIter->process.priority < scheduler->currentProcess->process.priority)
      scheduler->currentProcess = nodeIter;
    else if(!nextSamePriority)
      scheduler->currentProcess = nodeIter;
  }

  return scheduler->currentProcess->process.sp;
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
  scheduler->quantum = QUANTUM;

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
