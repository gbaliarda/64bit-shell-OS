#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void wait(double seconds) {
  int initialTicks = ticks_elapsed();
  while ((ticks_elapsed() - initialTicks) / (double) 18 < seconds)
    _hlt();
}
