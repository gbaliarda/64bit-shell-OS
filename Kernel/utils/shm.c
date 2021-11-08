#include "mm.h"

#define SHM_SIZE (1024*4)
#define MAX_SHM 20
#define NULL (void *)0

void *shm[MAX_SHM];

void shmInit() {
  for(int i = 0; i < MAX_SHM; i++)
    shm[i] = NULL;
}


void *shmAttach(int id) {
  if (id < 0 || id >= MAX_SHM)
    return NULL;

  if (shm[id])
    return shm[id];

  shm[id] = alloc(SHM_SIZE);
  return shm[id];
}


void shmClose(int id) {
  if (id < 0 || id >= MAX_SHM)
    return;
    
  free(shm[id]);
  shm[id] = NULL;
}