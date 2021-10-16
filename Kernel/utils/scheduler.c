#include "../include/scheduler.h"
#include "../include/naiveConsole.h"
#include "../include/mm.h"
#include "../include/lib.h"

#define QUANTUM 4
#define MAX_CANTIDAD_PROCESOS 10
#define NULL ((void*) 0)

typedef struct pcb {
  uint32_t pid;
  uint8_t pstate;
  uint8_t priority;
  uint64_t sp;
} pcb;

typedef struct ListNode {
  pcb process;
  struct ListNode *next; 
} ListNode;

typedef struct Scheduler {
  uint32_t quantum;
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
  scheduler->start = (ListNode *)((uint64_t)scheduler + sizeof(Scheduler));
  scheduler->start->next = NULL;
  pid = 0;
  firstProcess = 1;
}

static ListNode *loadProcess(ListNode * node, uint32_t pid, uint8_t priority, uint64_t sp) {
  if (node->next != NULL && node->next->process.priority > priority) {
    node->next = loadProcess(node->next, pid, priority, sp);
    return node;
  }

  ListNode *newNode = (ListNode *) ((uint64_t)node + sizeof(ListNode));

  newNode->next = node->next;
  node->next = newNode;
  
  newNode->process.pid = pid;
  newNode->process.pstate = 1; // por defecto pstate = 1 = ready
  newNode->process.priority = priority;
  newNode->process.sp = sp;
  return newNode;
}

void createProcess(uint64_t ip, uint32_t size, uint8_t priority) {
  uint64_t process = (uint64_t) alloc(size);
	uint64_t sp = initProcess(process + size, ip);
  scheduler->start = loadProcess(scheduler->start, pid++, priority, sp);
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

  // guardar el sp del proceso actual en su PCB
  scheduler->currentProcess->process.sp = sp;
  
  ListNode * nodeIter = scheduler->start;
  while(nodeIter != NULL && nodeIter->process.pstate != 1)
    nodeIter = nodeIter->next;

  // deberia haber un dummy para que siempre haya un proceso ready, por lo que esto no podria pasar
  if(nodeIter == NULL)
    return 0;

  scheduler->currentProcess = nodeIter;
  return nodeIter->process.sp;
}
