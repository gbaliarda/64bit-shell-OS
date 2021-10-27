#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../include/lib.h"
#include "../include/mm.h"
#include "../include/semaphore.h"

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

void initScheduler();

uint64_t switchProcess(uint64_t sp);

void createProcess(uint64_t ip, uint32_t size, uint8_t priority, uint64_t argc, char ** argv);

void exitCurrentProcess();

void printProcessList();

void killPid(uint32_t pid);

void changeProcessPriority(uint32_t pid, uint8_t newPriority);

void changeProcessState(uint32_t pid);

void waitForKeyboard();

void awakeKeyboardQueue();

pcb *blockCurrentProcess();

#endif