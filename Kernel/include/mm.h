#ifndef MM_H
#define MM_H

void initMemManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);

void *alloc(unsigned int memoryToAllocate);

void free(void *memoryToFree);

void memStatus(unsigned int *status);

#endif