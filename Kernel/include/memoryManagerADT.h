#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *alloc(MemoryManagerADT const memoryManager, unsigned int memoryToAllocate);

void free(MemoryManagerADT const memoryManager, void *memoryToFree);

#endif