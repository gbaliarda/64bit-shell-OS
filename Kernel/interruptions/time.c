#include <time.h>
#include "../include/scheduler.h"
#include "../include/naiveConsole.h"

static unsigned long ticks = 0;

typedef struct TimerNode {
  pcb *process;
  uint32_t seconds;
  uint32_t initialTicks;
  struct TimerNode *next;
} TimerNode;

TimerNode *start = NULL;

void addToQueue(uint32_t seconds) {
  TimerNode *newNode = (TimerNode *) alloc(sizeof(TimerNode));
  newNode->process = getCurrentProcess();
  newNode->seconds = seconds;
  newNode->initialTicks = ticks_elapsed();
  newNode->next = NULL;
  
  if (start == NULL)
    start = newNode;
  else {
    newNode->next = start;
    start = newNode;
  }
}

TimerNode *removeFromQueue(TimerNode* node, uint32_t pid) {
  if (node == NULL)
    return NULL;
  
  if (node->process->pid == pid) {
    ncPrint("Despertando a: ");
    ncPrintDec(node->process->pid);
    TimerNode *aux = node->next;
    node->process->pstate = 1;
    free(node);
    return aux;
  }
  
  node->next = removeFromQueue(node->next, pid);
  return node;
}

void timer_handler() {
	ticks++;
  TimerNode *iterator = start;
  TimerNode *prev = start;
  while (iterator != NULL) {
    if ((ticks_elapsed() - iterator->initialTicks) / 18 >= iterator->seconds) {
      ncPrint("Despertando a: ");
      ncPrintDec(iterator->process->pid);
      iterator->process->pstate = 1;
      if (iterator->process->pid == prev->process->pid) {
        free(start);
        start = NULL;
        break;
      }
      prev->next = iterator->next;
      free(iterator);
      iterator = prev->next;
      continue;
    }
    prev = iterator;
    iterator = iterator->next;
  }
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void sleep(uint32_t seconds) {
  addToQueue(seconds);
  blockCurrentProcess();
  runScheduler();
}
