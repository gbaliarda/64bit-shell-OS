#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

void initScheduler();

void loadProcess(uint64_t pcb);

uint64_t switchProcess(uint64_t sp);

#endif