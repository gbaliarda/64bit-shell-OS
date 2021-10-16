#include <stdint.h>
#include "syscalls.h"
#include "../include/mm.h"
#include "../include/scheduler.h"

static int sysNum;

void loadSysNum(int64_t rax) {
  sysNum = rax;
}

int64_t syscallDispatcher(int64_t arg0, int64_t arg1, int64_t arg2) {
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
        createProcess((uint64_t) arg0, (uint32_t) arg1, (uint8_t) arg2);
        return 1;

    default:
      return -1;
  }
}
