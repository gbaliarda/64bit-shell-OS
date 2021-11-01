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
#define SEM 50
#define N 100

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
  uint32_t pid = sys_getPid();
  int64_t aux = *p;
  aux += inc;
  for (int i = 0; i < 500000*(pid % 5 + 1); i++);
  // yield();
  *p = aux;
}

void inc(){
  uint64_t i;
  int64_t value = sys_getPid() % 2 ? 1 : -1;

  Semaphore *sem = my_sem_open(SEM, 1);

  if (sem == NULL){
    printf("ERROR OPENING SEM\n");
    return;
  }
  
  for (i = 0; i < N; i++) {
    my_sem_wait(sem);
    slowInc(&global, value);
    my_sem_post(sem);
  }
  
  char buff[20];
  printf("Final value: ");
  itos(global, buff, 0);
  printf(buff);
  printf("\n");
  sys_exit();
}

void ninc(){
  uint64_t i;
  int64_t value = sys_getPid() % 2 ? 1 : -1;

  for (i = 0; i < N; i++) {
    slowInc(&global, value);
  }
  
  char buff[20];
  printf("Final value: ");
  itos(global, buff, 0);
  printf(buff);
  printf("\n");
  sys_exit();
}

void test_sync(){
  uint64_t i;

  global = 0;

  Semaphore *sem = my_sem_open(SEM, 1);

  printf("CREATING PROCESSES...(WITH SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process("i");
    my_create_process("i");
  }

  my_sem_close(sem);
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
