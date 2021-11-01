#include "libc.h"
#include "philo.h"
#include "syscalls.h"

#define MAX_PH 10
#define EAT_TIME 1

//  ref: https://www.geeksforgeeks.org/dining-philosopher-problem-using-semaphores/

typedef struct
{
  int state;
  int pid;
} philo_t;

enum state
{
  THINKING,
  EATING,
  HUNGRY
};

static int deletePh(int index);

static void printTable();

static void philoProcess(int argc, const char argv[6][21]);

static void takeChopsticks(int index);

static void putChopsticks(int index);

static void checkAvailability(int index);

static int left(int index);

static int right(int index);

static void endDining();

static philo_t philos[MAX_PH]; 
static int amountPh; 
Semaphore *mutex;  
Semaphore *philoSem[MAX_PH];

static int addPh(int index) {
  if(index >= MAX_PH)
    return -1;
  
  sys_semWait(mutex);


  philos[index].state = THINKING;
  philoSem[index] = sys_semOpen(index+100, 1);
  if(philoSem[index] == NULL)
    return -1;

  char seat[3];
  uintToBase(index, seat, 10);
  char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1];
  strcpy(argv[0], "philo");
  strcpy(argv[1], seat);
  strcpy(argv[2], "&");
  
  philos[index].pid = createProcess((uint64_t)&philoProcess, 3, argv, NULL, NULL);
  amountPh++;
  sys_semPost(mutex);
  return 0;
}

static void philoProcess(int argc, const char argv[6][21]) {
  int ok = 1;
  int seat = atoi((char *)argv[1], &ok);
  while (1)
  {
    sys_sleep(EAT_TIME);
    takeChopsticks(seat);
    sys_sleep(EAT_TIME);
    putChopsticks(seat);
  }
}


static void takeChopsticks(int index) {
  sys_semWait(mutex);

  philos[index].state = HUNGRY;
  checkAvailability(index);

  sys_semPost(mutex);
  sys_semWait(philoSem[index]);

}


static void putChopsticks(int index) {
  sys_semWait(mutex);

  philos[index].state = THINKING;

  checkAvailability(left(index));
  checkAvailability(right(index));

  sys_semPost(mutex);
}

static void checkAvailability(int index) {
  if(philos[index].state == HUNGRY && philos[left(index)].state != EATING &&  philos[right(index)].state != EATING) {
    philos[index].state = EATING;
    printTable();

    sys_semPost(philoSem[index]);
  }

}

static int left(int index) {
  return (index == 0) ? amountPh - 1 : index - 1;
}

static int right(int index) {
  return (index == amountPh -1) ? 0 : index + 1;
}


static int deletePh(int index) {
  if(index < 0 || amountPh == 0)
    return -1;

  sys_semWait(mutex);

  int eating = (philos[index].state == EATING);

  if (sys_semClose(philoSem[index]) == -1) {
    printf("Error closing philo's sem");
    return -1;
  }

  sys_killProcess(philos[index].pid);
  amountPh--;

  if(eating) {
    checkAvailability(index - 1);
    checkAvailability(0);
  }

  sys_semPost(mutex);
  return 0;
}

static void printTable() {
  for (int i = 0; i < amountPh; i++)
  {
    philos[i].state == EATING ? printf(" E ") : printf(" . ");
  }
  printf("\n");
}

static void endDining() {
  while (amountPh > 0) 
  {
    if (sys_semClose(philoSem[amountPh - 1]) == -1)
      printf("Error closing philo's sem");

    sys_killProcess(philos[amountPh - 1].pid);

    amountPh--;
  }
}

void philo(int argc, const char argv[6][21]) {
  printf("Bienvenidos al juego de los filosofos!\n");
  mutex = sys_semOpen(99, 1);

  amountPh = 0;

  printf(" 'a' para agregar - 'r' para remover filosofos - 'q' para salir\n");
  char c[1];
  while( getChar(c) != 0 ) 
  {
    switch (c[0])
    {
    case 'a':
    case 'A':
      if(addPh(amountPh) == -1)
        printf("Error agregando filosofo\n");
      printf("\n");
      break;
    case 'r':
    case 'R':
      if(deletePh(amountPh - 1) == -1)
        printf("Error borrando filosofo\n");
      printf("\n");
      break;
    case 'q':
    case 'Q':
      printf("\n");
      endDining();
      sys_semClose(mutex);
      sys_exit();
    }
  }
  sys_semClose(mutex);
  sys_exit();
}