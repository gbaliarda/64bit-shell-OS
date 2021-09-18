#include <stdint.h>
#include "syscalls.h"

static int sysNum;

void loadSysNum(int64_t rax)
{
  sysNum = rax;
}

int64_t syscallDispatcher(int64_t arg0, int64_t arg1, int64_t arg2)
{
  switch (sysNum) {
    case 0:
        return read((char*) arg0, (int) arg1, (int*) arg2);
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

    default:
      return -1; 
  }
}
