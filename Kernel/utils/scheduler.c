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

void initScheduler() {
  scheduler = (Scheduler *) alloc(sizeof(Scheduler));
  scheduler->index = 0;
  scheduler->currentProcess = 0;
}

void loadProcess(uint64_t pcb) {
  scheduler->readyList[scheduler->index++] = (pcb_t *) (pcb - sizeof(pcb_t));
  ncPrint("PCB: ");
  ncPrintHex(pcb);
  ncNewline();
  ncPrint("SS: ");
  ncPrintHex(scheduler->readyList[scheduler->currentProcess]->stackSegment);
  ncNewline();
  ncPrint("SP: ");
  ncPrintHex(scheduler->readyList[scheduler->currentProcess]->sp);
  ncNewline();
  ncPrint("RFLAGS: ");
  ncPrintHex(scheduler->readyList[scheduler->currentProcess]->rflags);
  ncNewline();
  ncPrint("CS: ");
  ncPrintHex(scheduler->readyList[scheduler->currentProcess]->codeSegment);
  ncNewline();
  ncPrint("IP: ");
  ncPrintHex(scheduler->readyList[scheduler->currentProcess]->ip);
  ncNewline();
  ncPrint("ARGC: ");
  ncPrintHex(scheduler->readyList[scheduler->currentProcess]->argc);
  ncNewline();
  ncPrint("ARGV: ");
  ncPrintHex(scheduler->readyList[scheduler->currentProcess]->argv);
  ncNewline();
}

uint64_t switchProcess() {
  return scheduler->readyList[scheduler->currentProcess]->sp;
}
