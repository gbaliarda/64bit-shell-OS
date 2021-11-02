// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

void timer_handler() {
	ticks++;
  TimerNode *iterator = start;
  TimerNode *prev = start;
  while (iterator != NULL) {
    if ((ticks_elapsed() - iterator->initialTicks) / 18 >= iterator->seconds) {
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

static void fakePrint(const char * string) {
  return;
}

void sleep(uint32_t seconds) {
  addToQueue(seconds);
  blockCurrentProcess();
  fakePrint(" "); // por alguna razon no anda sin esto
  runScheduler();
}
