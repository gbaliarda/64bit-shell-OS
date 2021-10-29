#include "../include/semaphore.h"
#include "../include/naiveConsole.h"

#define PROCESS_LIMIT 10
#define MAX_SEM 20

typedef struct Semaphore {
  uint32_t id;
  int value;
  pcb *queue[PROCESS_LIMIT];
  uint32_t waiting;
} Semaphore;

Semaphore *semaphores[MAX_SEM];
uint32_t size = 0;

Semaphore *sem_open(uint32_t id, int value) {
  if (size == MAX_SEM)
    return NULL;

  int semIterator = 0;
  while(semIterator < size && semaphores[semIterator]->id != id)
    semIterator++;

  if(semIterator == size) {
    semaphores[size] = (Semaphore *) alloc(sizeof(Semaphore));
    semaphores[size]->id = id;
    semaphores[size]->value = value;
    semaphores[size]->waiting = 0;
    return semaphores[size++];
  }
  return semaphores[semIterator];
}

int sem_close(Semaphore *sem) {
  int semIterator = 0;
  while (semIterator < size && sem->id != semaphores[semIterator]->id)
    semIterator++;

  if(semIterator == size)
    return -1;
  
  while(semIterator < size-1)
    semaphores[semIterator] = semaphores[semIterator+1];
  size--;

  free(sem);
  return 0;
}

int sem_wait(Semaphore *sem) {
  if (sem->value <= 0) {
    if(sem->waiting == PROCESS_LIMIT)
      return -1;
    
    sem->value--;
    sem->queue[sem->waiting++] = blockCurrentProcess();
    runScheduler();
  } else
   sem->value--; 

  return 0;
}

int sem_post(Semaphore *sem) {
  sem->value++;

  if(sem->value < 0 || sem->waiting == 0)
    return 0;
  
  sem->queue[0]->pstate = 1;
  sem->waiting--;
  for (int i = 0; i < sem->waiting; i++)
    sem->queue[i] = sem->queue[i+1];
  return 0;
}

void printSemaphores() {
  uint32_t index = 0;
  while(index < size) {
    ncPrint("ID      Value       Blocked Process\n");
    ncPrintDec(semaphores[index]->id);
    ncPrint("       ");
    ncPrintDec(semaphores[index]->value);
    ncPrint("             ");
    if(semaphores[index]->waiting > 0) {
      ncPrintDec(semaphores[index]->queue[0]->pid);
      ncPrint("\n");
      for(int i = 1; i < semaphores[index]->waiting; i++) {
        ncPrint("                     ");
        ncPrintDec(semaphores[index]->queue[i]->pid);
        ncPrint("\n");
      }
    } else {
      ncPrint("None");
      ncPrint("\n");
    }
    index++;
  }
}

void deleteProcessFromSemaphores(uint32_t pid) {
  for (int i = 0; i < size; i++)
    for (int j = 0; j < semaphores[i]->waiting; j++) {
      if (semaphores[i]->queue[j]->pid == pid) {
        semaphores[i]->waiting--;
        for (int k = 0; i < semaphores[i]->waiting; i++)
          semaphores[i]->queue[k] = semaphores[i]->queue[k+1];
        return;
      }
    }
}