#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

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

#endif