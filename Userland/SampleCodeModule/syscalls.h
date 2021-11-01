#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "./libc.h"

void sys_write(const char *buffer, unsigned int count);

int sys_read(char *buffer, int limit);

void sys_clearScreen();

void *sys_alloc(unsigned int memoryToAllocate);

void sys_free(void *memoryToFree);

void sys_memStatus(unsigned int *status);

int sys_createProcess(uint64_t ip, uint8_t priority, unsigned int argc, char *argv, fdPipe *customStdin, fdPipe *customStdout);

void sys_exit();

void sys_printProcess();

void sys_killProcess(uint32_t pid);

void sys_changePriority(uint32_t pid, uint8_t newPriority);

void sys_changeState(uint32_t pid);

Semaphore *sys_semOpen(uint32_t id, int value);

int sys_semClose(Semaphore *sem);

int sys_semWait(Semaphore *sem);

int sys_semPost(Semaphore *sem);

void sys_printSemaphores();

int sys_createPipe(fdPipe *f0, fdPipe *f1);

fdPipe *sys_createFdPipe();

void sys_closeFdPipe(fdPipe *fd);

int sys_pipeWrite(fdPipe *fd, char* string);

int sys_pipeRead(fdPipe *fd, char* buffer);

int sys_openPipeId(fdPipe *fd, uint32_t id, uint8_t reader);

void sys_printPipes();

void sys_yield();

uint32_t sys_getPid();

void sys_sleep(unsigned int seconds);

#endif