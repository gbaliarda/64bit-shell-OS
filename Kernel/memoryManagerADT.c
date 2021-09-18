#include "./include/memoryManagerADT.h"

typedef struct MemoryManagerCDT {
  char *current;
} MemoryManagerCDT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
  MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->current = managedMemory;

	return memoryManager;
}

void *alloc(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {
  char *tmp = memoryManager->current;
	memoryManager->current += memoryToAllocate;
	return (void *) tmp;
}