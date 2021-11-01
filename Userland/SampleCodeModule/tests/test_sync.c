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

#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID 50
#define INC_TIMES 100
#define INC_AMOUNT 1

int64_t global;  //shared memory

static void slowInc(int64_t *p, int64_t inc){
  int64_t aux = *p;
  aux += inc;
  sys_yield();
  *p = aux;
}

static void inc(){
  uint64_t i;

  Semaphore *sem = my_sem_open(SEM_ID, 1);

  if (sem == NULL){
    printf("ERROR OPENING SEM\n");
    return;
  }
  
  for (i = 0; i < INC_TIMES; i++){
    my_sem_wait(sem);
    slowInc(&global, INC_AMOUNT);
    my_sem_post(sem);
  }

  my_sem_close(sem);
  
  printf("Final value: ");
  printInt(global);
  printf("\n");
  sys_exit();
}

static void ninc(){
  uint64_t i;
  
  for (i = 0; i < INC_TIMES; i++){
    slowInc(&global, INC_AMOUNT);
  }
  
  printf("Final value: ");
  printInt(global);
  printf("\n");
  sys_exit();
}

void test_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...(WITH SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process("i");
    my_create_process("i");
  }
}

void test_no_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...(WITHOUT SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process("n");
    my_create_process("n");
  }
}