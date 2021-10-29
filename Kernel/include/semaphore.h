#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "../include/scheduler.h"

typedef struct Semaphore* semaphorePointer;

semaphorePointer sem_open(uint32_t id, int value);

int sem_close(semaphorePointer sem);

int sem_wait(semaphorePointer sem);

int sem_post(semaphorePointer sem);

void printSemaphores();

void deleteProcessFromSemaphores(uint32_t pid);

extern uint8_t _xchg(uint8_t *lock, int value); 

#endif