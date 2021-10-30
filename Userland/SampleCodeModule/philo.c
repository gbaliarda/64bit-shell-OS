#include "libc.h"

#define MAX_PH 15
#define MIN_PH 2
#define INIT_PH 5
#define EAT_TIME 3


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


static philo_t philos[MAX_PH]; 
static int amountPh; 
Semaphore *mutex;  // Para que ningun filosofo pueda agarrar palitos o dejar palitos a la vez 
Semaphore *philoSem[MAX_PH];

// Funcion que ejecuta el problema de los filosofos 
void philo() {
  printf("Bienvenidos al juego de los filosofos!\n");
  mutex = sys_semOpen(15, 1);

  amountPh = 0;
  for (int i = 0; i < MIN_PH; i++)
  {
    if (addPh(i) == -1)
      printf("Error crear filosofo!\n");
  }

  printf(" 'a' para agregar - 'q' para remover filosofos - 'q' para salir");
  char c;
  while( getChar(c) != 0 ) 
  {
    switch (c)
    {
    case 'a':
    case 'A':
      if(addPh(amountPh) == -1)
        printf("Error agregando filosofo");
      break;
    case 'r':
    case 'R':
      if(deletePh(amountPh - 1) == -1)
        printf("Error borrando filosofo");
      break;
    case 'q':
    case 'Q':
      endDining();
    
    return;
    }
  }
}

//Agrego filosofo a la mesa y agrego un chopstick (sem)
static int addPh(int index) {
  //Error
  if(index < MIN_PH -1 || index >= MAX_PH)
    return -1;

  
  sys_semWait(mutex);

  amountPh++;
  //Agrego a la lista de los filosofos
  philos[index].state = THINKING;
  //Agrego un nuevo chopstick como recurso por la llegada de un nuevo filosofo
  philoSem[index] = sys_semOpen(index, 1);

  char seat[3];
  uintToBase(index, seat, 3);
  char *argv[] = { "philo", seat };
  philos[index].pid = sys_createProcess((uint64_t)&philoProcess, 2048, 2, 2, argv);
  sys_semPost(mutex);

  return 0;
}

/* proceso filosofo que busca chopsticks e intenta comer si puede, sino espera */
void philoProcess(int argc, char  *argv[]) {
  int ok = 1;
  int seat = atoi(argv[1], &ok);

  while (1)
  {
    takeChopsticks(seat);
    eat();
    putChopstickes(seat);
  }
}

// El filoso trata de agarrar dos palitos a la vez, sino espera 
// Al agarrar dos palitos a la vez nos evitamos los abrazos mortales
void takeChopsticks(int index) {
  sys_semWait(mutex);

  philos[index].state = HUNGRY;
  checkAvailability(index);

  sys_semPost(mutex);
  //Si no pudo comer espera a ser llamado
  sys_semWait(philoSem[index]);

}

//El filoso termino de comer devuelve los palitos 
void putChopsticks(int index) {
  sys_semWait(mutex);

  philos[index].state = THINKING;

  checkAvailability(left(index));
  checkAvailability(right(index));

  sys_semPost(mutex);
}

// No esta definido wait creo (esta en time.c la que quiero usar)
void eat(){
  wait((double)EAT_TIME);
}

void checkAvailability(index) {
  // Veo si los dos filosofos de al lado no quieren comer 
  if(philos[index].state == HUNGRY && philos[left(index)].state == THINKING &&  philos[right(index)].state == THINKING) {
    philos[index].state = EATING;
    printTable();

    // Para llamar a los que no pudieron comer en el putChopsticks 
    sys_semPost(index);
  }

}

int left(int index) {
  return (index + 1) % amountPh;
}

int right(int index) {
  return ((index == 0 ? amountPh -1 : index -1));
}


static int deletePh(int index){
  sys_semWait(mutex);

  amountPh--;
  int eating = (philos[index].state == EATING);

  if (sys_semClose(philoSem[index]) == -1)
    printf("Error closing philo's sem");
  //Validar el kill ?
  sys_killProcess(philos[index].pid);

  // Como siempre es el ultimo si llego a comer y justo lo cerramos tenemos que actualizar la fila avisandole al anteultimo y al primero
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
  putChar("\n");
}

void endDining() {
  while (amountPh > 0) 
  {
    if (sys_semClose(philoSem[amountPh - 1]) == -1)
      printf("Error closing philo's sem");

    sys_killProcess(philos[amountPh - 1].pid);

    amountPh--;
  }
}