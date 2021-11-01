#include "tests.h"
#include "../syscalls.h"
#include "../libc.h"

static void inc();

static void ninc();

static int my_create_process(char * name){
  char argv[6][21];
  argv[1][0] = '&';
  argv[1][1] = 0;


  if(name[0] == 'i')
    return createProcess((uint64_t)&inc, 2, argv, NULL, NULL);
  else
    return createProcess((uint64_t)&ninc, 2, argv, NULL, NULL);
}

static Semaphore *my_sem_open(uint32_t sem_id, uint64_t initialValue){
  return sys_semOpen(sem_id, initialValue);
}

static int my_sem_wait(Semaphore *sem){
  return sys_semWait(sem);
}

static uint64_t my_sem_post(Semaphore *sem){
  return sys_semPost(sem);
}

static int my_sem_close(Semaphore *sem){
  return sys_semClose(sem);
}

#define TOTAL_PROCESSES 5
#define SEM_ID 50
#define INC_TIMES 3
#define INC_AMOUNT 1

static void inc(){
  uint64_t i;

  Semaphore *sem = my_sem_open(SEM_ID, 1);

  if (sem == NULL){
    printf("ERROR OPENING SEM\n");
    return;
  }
  
  for (i = 0; i < INC_TIMES; i++){
    if (my_sem_wait(sem) == -1)
      printf("Error waiting\n");
    printf("I am ");
    printInt(sys_getPid());
    printf(" entering critical zone\n");
    sys_sleep(1);
    printf("Exiting critical zone\n");
    if (my_sem_post(sem) == -1)
      printf("Error posting\n");
    sys_sleep(1);
  }

  if (my_sem_close(sem) == -1) {
    printf("Error closing sem: Sem in use or sem already closed\n");
  }
  printf("Exiting process ");
  printInt(sys_getPid());
  printf("\n");
  sys_exit();
}

static void ninc(){
  uint64_t i;
  
  for (i = 0; i < INC_TIMES; i++){
    printf("I am ");
    printInt(sys_getPid());
    printf(" acceding critical zone\n");
    sys_sleep(1);
    printf("Exiting critical zone\n");
    sys_sleep(1);
  }
  
  printf("Exiting process ");
  printInt(sys_getPid());
  printf("\n");
  sys_exit();
}

void test_sync(){
  uint64_t i;

  printf("CREATING PROCESSES...(WITH SEM)\n");

  for(i = 0; i < TOTAL_PROCESSES; i++){
    my_create_process("i");
  }
}

void test_no_sync(){
  uint64_t i;

  printf("CREATING PROCESSES...(WITHOUT SEM)\n");

  for(i = 0; i < TOTAL_PROCESSES; i++){
    my_create_process("n");
  }
}