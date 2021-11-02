// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./syscalls.h"
#include "./builtin.h"

void help(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  printf("-------BUILTIN------\n");
  printf("1: block\n");
  printf("2: clear\n");
  printf("3: help\n");
  printf("4: kill\n");
  printf("5: mem\n");
  printf("6: nice\n");
  printf("7: pipe\n");
  printf("8: ps\n");
  printf("9: sem\n");
  printf("-------PROCESSES------\n");
  printf("1: cat\n");
  printf("2: filter\n");
  printf("3: loop\n");
  printf("4: phylo\n");
  printf("5: wc\n");
  printf("-------TESTS------\n");
  printf("1: testmm\n");
  printf("2: testprio\n");
  printf("3: testproc\n");
  printf("4: testsync\n");
  printf("5: testnosync\n");
}

void clear(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  sys_clearScreen();
}

void ps(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  sys_printProcess();
}

void kill(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  int ok = 1;
  sys_killProcess((uint32_t) atoi(args[1], &ok));
  if (!ok)
    printf("Invalid PID\n");
}

void nice(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  int ok = 1;
  sys_changePriority((uint32_t) atoi(args[1], &ok), (uint8_t) atoi(args[2], &ok));
  if (!ok)
    printf("Invalid parameters\n");
}

void block(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  int ok = 1;
  sys_changeState((uint32_t) atoi(args[1], &ok));
  if (!ok)
    printf("Invalid PID\n");
}

void sem(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  sys_printSemaphores();
}

void pipe(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  sys_printPipes();
}

void mem(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
  unsigned int mem[3];
  sys_memStatus(mem);
  printf("Heap size: ");
  printInt(mem[0]);
  printf(" bytes\n");
  printf("Heap left: ");
  printInt(mem[1]);
  printf(" bytes\n");
  printf("Heap used: ");
  printInt(mem[2]);
  printf(" bytes\n");
}
