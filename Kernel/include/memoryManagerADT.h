#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#define TOTAL_HEAP_SIZE 134217728 // 128MB

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMem(MemoryManagerADT const memoryManager, unsigned int memoryToAllocate);

void freeMem(MemoryManagerADT const memoryManager, void *memoryToFree);

unsigned int heapSize();

unsigned int heapLeft(MemoryManagerADT mm);

unsigned int usedHeap(MemoryManagerADT mm);

#endif