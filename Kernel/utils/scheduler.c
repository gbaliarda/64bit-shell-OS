#include "../include/scheduler.h"
#include "../include/naiveConsole.h"
#include "../include/mm.h"

typedef struct pcb_t {
  uint64_t argv;
  uint64_t argc;
  uint64_t ip;
  uint64_t codeSegment;
  uint64_t rflags;
  uint64_t sp;
  uint64_t stackSegment;
} pcb_t;

typedef struct Scheduler {
  pcb_t *readyList[10];
  uint32_t index;
  uint32_t currentProcess;
} Scheduler;

Scheduler *scheduler;
uint8_t firstProcess;

void initScheduler() {
  scheduler = (Scheduler *) alloc(sizeof(Scheduler));
  scheduler->index = 0;
  scheduler->currentProcess = 0;
  firstProcess = 1;
}

void loadProcess(uint64_t pcb) {
  scheduler->readyList[scheduler->index++] = (pcb_t *) (pcb - sizeof(pcb_t));
}

uint64_t switchProcess(uint64_t sp) {
  // Para el primer proceso no actualizamos el sp, usamos el hardcodeado
  if (firstProcess)
    firstProcess = 0;
  else
    // guardar el sp del proceso actual en su PCB
    scheduler->readyList[scheduler->currentProcess]->sp = sp;
  ncPrint("Switching!\n");
  // retornar el sp del proximo proceso a ejecutarse
  return scheduler->readyList[scheduler->currentProcess]->sp;
}
