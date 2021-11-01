#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../include/lib.h"
#include "../include/mm.h"
#include "../include/semaphore.h"

typedef struct fdPipe {
  char readable;
  char writable;
  struct pipe *pipe;
} fdPipe;

typedef struct pcb {
  char args[6][21];
  uint32_t pid;
  uint8_t pstate;
  uint8_t priority;
  uint8_t auxPriority;
  uint64_t sp;
  uint64_t bp;
  uint64_t processMemory;
  fdPipe *customStdin;
  fdPipe *customStdout;
} pcb;


void initScheduler();

uint64_t switchProcess(uint64_t sp);

int createProcess(uint64_t ip, uint8_t priority, uint64_t argc, char args[6][21], fdPipe *customStdin, fdPipe *customStdout);

int createProcessWrapper(uint64_t ip, uint8_t priority, uint64_t argc, char * argv, fdPipe *customStdin, fdPipe *customStdout);

void exitCurrentProcess();

void printProcessList();

void killPid(uint32_t pid);

void changeProcessPriority(uint32_t pid, uint8_t newPriority);

void changeProcessState(uint32_t pid);

void waitForKeyboard();

void awakeKeyboardQueue();

pcb *blockCurrentProcess();

fdPipe *getCurrentStdin();

fdPipe *getCurrentStdout();

uint32_t getCurrentPid();

pcb *getCurrentProcess();

#endif