#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#define TOTAL_HEAP_SIZE 134217728 // 128MB

typedef unsigned int size_t;
typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMem(MemoryManagerADT const memoryManager, size_t memoryToAllocate);

void freeMem(MemoryManagerADT const memoryManager, void *memoryToFree);

size_t heapSize();

size_t heapLeft(MemoryManagerADT mm);

size_t usedHeap(MemoryManagerADT mm);

#endif