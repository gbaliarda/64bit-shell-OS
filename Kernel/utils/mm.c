// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/mm.h"
#include "../include/memoryManagerADT.h"

static MemoryManagerADT mm;

void initMemManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
  mm = createMemoryManager(memoryForMemoryManager, managedMemory);
}

void *alloc(unsigned int memoryToAllocate) {
  return allocMem(mm, memoryToAllocate);
}

void free(void *memoryToFree) {
  return freeMem(mm, memoryToFree);
}

void memStatus(unsigned int *status) {
  status[0] = heapSize();
  status[1] = heapLeft(mm);
  status[2] = usedHeap(mm);
}
