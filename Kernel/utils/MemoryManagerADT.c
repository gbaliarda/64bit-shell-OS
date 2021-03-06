// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifdef HEAP2

#include "../include/memoryManagerADT.h"
#include <stdint.h>

// Source: https://github.com/Infineon/freertos/blob/master/Source/portable/MemMang/heap_2.c

#define NULL ((void *) 0)

typedef struct MemoryBlock {
	struct MemoryBlock *nextFreeBlock;
	unsigned int blockSize;
} MemoryBlock;

static const uint16_t STRUCT_SIZE = ((sizeof(MemoryBlock) + (BYTE_ALIGNMENT - 1)) & ~BYTE_ALIGNMENT_MASK);

#define MINIMUM_BLOCK_SIZE ((unsigned int)(STRUCT_SIZE * 2))

typedef struct MemoryManagerCDT {
  MemoryBlock start;
  MemoryBlock end;
  unsigned int freeBytesRemaining;
} MemoryManagerCDT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
  MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
  memoryManager->freeBytesRemaining = TOTAL_HEAP_SIZE;

  MemoryBlock *firstFreeBlock = (void *) managedMemory;

	memoryManager->start.nextFreeBlock = (void *) firstFreeBlock;
	memoryManager->start.blockSize = (unsigned int) 0;

	memoryManager->end.blockSize = TOTAL_HEAP_SIZE;
	memoryManager->end.nextFreeBlock = NULL;

	firstFreeBlock->blockSize = TOTAL_HEAP_SIZE;
	firstFreeBlock->nextFreeBlock = &memoryManager->end;
  
	return memoryManager;
}

static void insertBlockIntoFreeList(MemoryManagerADT memoryManager, MemoryBlock *blockToInsert) {
  MemoryBlock *blockIterator = &memoryManager->start;
  unsigned int blockSize = blockToInsert->blockSize;
						
	while (blockIterator->nextFreeBlock->blockSize < blockSize)
    blockIterator = blockIterator->nextFreeBlock;
							
	blockToInsert->nextFreeBlock = blockIterator->nextFreeBlock;
	blockIterator->nextFreeBlock = blockToInsert;
}

void *allocMem(MemoryManagerADT const memoryManager, unsigned int memoryToAllocate) {

  MemoryBlock *block, *previousBlock;
  void *blockToReturn = NULL;

  if (memoryToAllocate == 0)
    return NULL;

  memoryToAllocate += STRUCT_SIZE;
    
  if ((memoryToAllocate & BYTE_ALIGNMENT_MASK) != 0) {
    memoryToAllocate += (BYTE_ALIGNMENT - (memoryToAllocate & BYTE_ALIGNMENT_MASK));
  }

  if (memoryToAllocate < TOTAL_HEAP_SIZE) {
    previousBlock = &memoryManager->start;
    block = memoryManager->start.nextFreeBlock;

    while ((block->blockSize < memoryToAllocate ) && (block->nextFreeBlock != NULL)) {
      previousBlock = block;
      block = block->nextFreeBlock;
    }

    if (block == &memoryManager->end)
      return NULL;

    blockToReturn = (void *) (((uint8_t *) previousBlock->nextFreeBlock) + STRUCT_SIZE);

    previousBlock->nextFreeBlock = block->nextFreeBlock;

    if ((block->blockSize - memoryToAllocate) > MINIMUM_BLOCK_SIZE) {
      MemoryBlock *newBlock = (void *) (((uint8_t *) block) + memoryToAllocate);

      newBlock->blockSize = block->blockSize - memoryToAllocate;
      block->blockSize = memoryToAllocate;

      insertBlockIntoFreeList(memoryManager, newBlock);
    }

    memoryManager->freeBytesRemaining -= block->blockSize;
  }

	return blockToReturn;
}

void freeMem(MemoryManagerADT const memoryManager, void *block) {
  if (block == NULL)
    return;

  uint8_t *memoryToFree = (uint8_t *) block;
  MemoryBlock *blockToFree;

  memoryToFree -= STRUCT_SIZE;
  blockToFree = (void *) memoryToFree;

  insertBlockIntoFreeList(memoryManager, ((MemoryBlock *) blockToFree));
  memoryManager->freeBytesRemaining += blockToFree->blockSize;

}

unsigned int heapSize() {
  return TOTAL_HEAP_SIZE;
}

unsigned int heapLeft(MemoryManagerADT mm) {
  return mm->freeBytesRemaining;
}

unsigned int usedHeap(MemoryManagerADT mm) {
  return heapSize() - heapLeft(mm);
}

#endif