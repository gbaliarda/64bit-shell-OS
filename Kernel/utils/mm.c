#include "../include/mm.h"
#include "../include/memoryManagerADT.h"

static MemoryManagerADT mm;

void initMemManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
  mm = createMemoryManager(memoryForMemoryManager, managedMemory);
}

void *alloc(size_t memoryToAllocate) {
  return allocMem(mm, memoryToAllocate);
}

void free(void *memoryToFree) {
  return freeMem(mm, memoryToFree);
}

void memStatus(size_t *status) {
  status[0] = heapSize();
  status[1] = heapLeft(mm);
  status[2] = usedHeap(mm);
}
