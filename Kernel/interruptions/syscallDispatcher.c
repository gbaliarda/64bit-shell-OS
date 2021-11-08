// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "syscalls.h"
#include "./time.h"
#include "../include/mm.h"
#include "../include/scheduler.h"
#include "../include/semaphore.h"
#include "../include/pipes.h"
#include "../include/shm.h"
#include "../include/naiveConsole.h"

static int sysNum;

void loadSysNum(int64_t rax) {
  sysNum = rax;
}

int64_t syscallDispatcher(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) {
  switch (sysNum) {
    case 0:
        return read((char*) arg0, (int) arg1);
    case 1:
        return write((char*) arg0, (uint64_t) arg1);
    case 10:
        inforeg((Registers *)arg0);
        return 1;
    case 11:
        printmem((uint64_t)arg0);
        return 1;
    case 12:
        getDateTime((Time *)arg0);
        return 1;
    case 13:
        clearScreen();
        return 1;
    case 14:
        return (int64_t) alloc((uint32_t) arg0);
    case 15:
        free((void *) arg0);
        return 1;
    case 16:
        memStatus((uint32_t *) arg0);
        return 1;
    case 17:
        return createProcessWrapper((uint64_t) arg0, (uint8_t) arg1, (uint32_t) arg2, (char *)arg3, (fdPipe *) arg4, (fdPipe *) arg5);
    case 18:
        exit();
        return 1;
    case 19:
        printProcess();
        return 1;
    case 20:
        killProcess((uint32_t) arg0);
        return 1;
    case 21:
        changePriority((uint32_t) arg0, (uint8_t) arg1);
        return 1;
    case 22:
        changeState((uint32_t) arg0);
        return 1;
    case 23:
        return (int64_t) sem_open((uint32_t) arg0, (int) arg1);
    case 24:
        return sem_close((semaphorePointer) arg0);
    case 25:
        return sem_wait((semaphorePointer) arg0);
    case 26:
        return sem_post((semaphorePointer) arg0);
    case 27:
        printSemaphores();
        return 1;
    case 28:
        return createPipe((fdPipe *) arg0, (fdPipe *) arg1);
    case 29:
        return (int64_t) createFdPipe();
    case 30:
        closeFdPipe((fdPipe *) arg0);
        return 1;
    case 31:
        return pipeWrite((fdPipe *) arg0, (char *) arg1);
    case 32: 
        return pipeRead((fdPipe *) arg0, (char *) arg2, 0);
    case 33: 
        return openPipeId((fdPipe *) arg0, (uint32_t) arg1, (uint8_t) arg2);
    case 34:
        printPipes();
        return 1;
    case 35:
        runScheduler();
        return 1;
    case 36:
        return getCurrentPid();
    case 37:
        sleep((uint32_t) arg0);
        return 1;
    case 38:
        return (int64_t) shmAttach((int) arg0);
    case 39:
        shmClose((int) arg0);
        return 1;
    default:
      return -1;
  }
}
