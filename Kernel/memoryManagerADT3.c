#include "./include/memoryManagerADT3.h"
#define BLOCK_SIZE 256
#define MAX_NBLOCKS 128
#define uint64_t unsigned long
#define NULL (void *)0

typedef struct memoryNode {
  unsigned char blockData;
  struct memoryNode *next;
} memoryNode;

typedef struct MemoryManagerCDT3 {
  memoryNode *first;
  memoryNode *last;
  char *endMemoryManager;
} MemoryManagerCDT3;

static void initializeBlocks(MemoryManagerADT3 const memoryManager);

MemoryManagerADT3 createMemoryManager3(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
  MemoryManagerADT3 memoryManager = (MemoryManagerADT3) memoryForMemoryManager;
	memoryManager->first = managedMemory;
	memoryManager->last = managedMemory;
  memoryManager->endMemoryManager = managedMemory+BLOCK_SIZE*MAX_NBLOCKS;

  initializeBlocks(memoryManager);

	return memoryManager;
}

static void initializeBlocks(MemoryManagerADT3 const memoryManager) {
  int i;
  memoryNode *aux = memoryManager->first;


  for (i = 0; i < MAX_NBLOCKS; i++) {
    aux->blockData = 2;
    aux->next = (memoryNode *)((uint64_t)aux+BLOCK_SIZE);
    aux = aux->next;
  }
}

void *alloc3(MemoryManagerADT3 const memoryManager, const unsigned int memoryToAllocate) {
  int nBlocks = (memoryToAllocate/BLOCK_SIZE)+1;

  if((char *)memoryManager->last + BLOCK_SIZE*nBlocks >= memoryManager->endMemoryManager)
    return NULL;

  memoryNode *aux = memoryManager->last->next;
  memoryNode *lastAux = memoryManager->last;

  int i = 1;
  while( i < nBlocks && (char *)aux < memoryManager->endMemoryManager ) {
    if( aux->blockData & 1 ) {
      aux = (memoryNode *)((uint64_t)aux + ((aux->blockData & 254) >> 1)*BLOCK_SIZE);
      lastAux = aux;
      i = 0;
    } else {
      aux = aux->next;
      i++;
    }
  }

  if ((char *)aux >= memoryManager->endMemoryManager)
    return NULL;

  lastAux->blockData = (nBlocks<<1) +1;
  memoryManager->last = aux;

	return (void *) lastAux+sizeof(memoryNode);
}

void free3(MemoryManagerADT3 const memoryManager, void *memoryToFree) {
  if ( memoryToFree == NULL || (memoryNode *) memoryToFree < memoryManager->first || (char *) memoryToFree >= memoryManager->endMemoryManager)
    return;

  memoryNode * aux = (memoryNode *) (memoryToFree-sizeof(memoryNode));

  if (memoryManager->last > aux)
    memoryManager->last = aux;

  if (aux->blockData & 1) {
    aux->blockData -= 1;
    int nBlocksToFree = (aux->blockData & 254) >> 1, i = 1;
    aux = aux->next;
    while( i < nBlocksToFree) {
      aux->blockData = 2;
      aux = aux->next;
      i++;
    }
  }

}