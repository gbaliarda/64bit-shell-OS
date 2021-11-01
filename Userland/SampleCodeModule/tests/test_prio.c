#include "../syscalls.h"
#include "../libc.h"
#include "tests.h"

#define MINOR_WAIT 10000000                               // TODO: To prevent a process from flooding the screen
#define WAIT      2000000000                              // TODO: Long enough to see theese processes beeing run at least twice

static uint64_t my_getpid(){
  return sys_getPid();
}

static void endless_loop();

static uint64_t my_create_process(char * name){
  char argv[6][21];
  argv[1][0] = '&';
  argv[1][1] = 0;
  return createProcess((uint64_t)&endless_loop, 2, argv, NULL, NULL);
}

static uint64_t my_nice(uint64_t pid, uint64_t newPrio){
  sys_changePriority(pid, newPrio);
  return 0;
}

static uint64_t my_kill(uint64_t pid){
  sys_killProcess(pid);
  return 0;
}

static uint64_t my_block(uint64_t pid){
  sys_changeState(pid);
  return 0;
}

static uint64_t my_unblock(uint64_t pid){
  sys_changeState(pid);
  return 0;
}

static void bussy_wait(uint64_t n){
  uint64_t i;
  for (i = 0; i < n; i++);
}

static void endless_loop(){
  uint64_t pid = my_getpid();

  while(1){
    printInt(pid);
    printf("");
    bussy_wait(MINOR_WAIT);
  }
}

#define TOTAL_PROCESSES 3

void test_prio(){
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for(i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = my_create_process("endless_loop");

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");


  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 6); //lowest priority 
        break;
      case 1:
        my_nice(pids[i], 4); //medium priority
        break;
      case 2:
        my_nice(pids[i], 2); //highest priority
        break;
    }
  }

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_block(pids[i]);


  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");
  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 3); //medium priority 
        break;
      case 1:
        my_nice(pids[i], 3); //medium priority
        break;
      case 2:
        my_nice(pids[i], 3); //medium priority
        break;
    }
  }

  printf("UNBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_kill(pids[i]);
}