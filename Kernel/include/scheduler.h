#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

void initScheduler();

uint64_t switchProcess(uint64_t sp);

void createProcess(uint64_t ip, uint32_t size, uint8_t priority);

void exitCurrentProcess();

#endif