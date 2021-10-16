#include "../include/scheduler.h"
#include "../include/naiveConsole.h"
#include "../include/mm.h"
#include "../include/lib.h"

#define QUANTUM 4
// #define MAX_CANTIDAD_PROCESOS 10

typedef struct pcb {
  uint32_t pid;
  uint8_t pstate;
  uint8_t priority;
  uint64_t sp;
} pcb;

// typedef struct ListNode {
//   pcb process;
//   struct ListNode *next; 
// } ListNode;

typedef struct Scheduler {
  pcb processList[10];
  // ListNode *start;
  uint32_t index;
  uint32_t currentProcess;
  uint32_t quantum;
} Scheduler;

Scheduler *scheduler;
uint8_t firstProcess;
uint32_t pid = 0;

void initScheduler() {
  scheduler = (Scheduler *) alloc(sizeof(Scheduler));
  scheduler->index = 0;
  scheduler->currentProcess = 0;
  scheduler->quantum = QUANTUM;
  // scheduler->start = (ListNode *)((uint64_t)scheduler + sizeof(Scheduler));
  firstProcess = 1;
}

static void loadProcess(uint32_t pid, uint8_t pstate, uint8_t priority, uint64_t sp) {
  scheduler->processList[scheduler->index].pid = pid;
  scheduler->processList[scheduler->index].pstate = pstate;
  scheduler->processList[scheduler->index].priority = priority;
  scheduler->processList[scheduler->index++].sp = sp;
}

void createProcess(uint64_t ip, uint32_t size, uint8_t priority) {
  uint64_t process = (uint64_t) alloc(size);
	uint64_t sp = initProcess(process + size, ip);
  loadProcess(pid++, 1, priority, sp);
}

uint64_t switchProcess(uint64_t sp) {
  // Para el primer proceso no actualizamos el sp, usamos el hardcodeado
  if (firstProcess) {
    firstProcess = 0;
    return scheduler->processList[scheduler->currentProcess].sp;
  }
  if(scheduler->quantum > 0) {
    scheduler->quantum--;
    return 0;
  }
  scheduler->quantum = QUANTUM;
  // guardar el sp del proceso actual en su PCB
  scheduler->processList[scheduler->currentProcess].sp = sp;

  // ROUND ROBIN CON PRIORIDADES

  // A -> B -> C -> D ;

  // retornar el sp del proximo proceso a ejecutarse
  return scheduler->processList[scheduler->currentProcess].sp;
}
