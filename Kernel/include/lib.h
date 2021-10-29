#ifndef LIB_H
#define LIB_H

#include <stdint.h>

#define NULL ((void*)0)

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

uint64_t initProcess(uint64_t stackBase, uint64_t ip, uint32_t argc, char **argv);

void runScheduler();

#endif