#include <stdint.h>
#include "syscalls.h"
#include "../include/mm.h"
#include "../include/scheduler.h"

static int sysNum;

void loadSysNum(int64_t rax) {
  sysNum = rax;
}

int64_t syscallDispatcher(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4) {
  switch (sysNum) {
    case 0:
        return read((char*) arg0, (int) arg1);
    case 1:
        return write((uint64_t) arg0, (const char*) arg1, (uint64_t) arg2);
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
        createProcess((uint64_t) arg0, (uint32_t) arg1, (uint8_t) arg2, (uint32_t) arg3, (char **) arg4);
        return 1;
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

    default:
      return -1;
  }
}
