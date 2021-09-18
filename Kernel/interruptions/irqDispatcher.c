#include <time.h>
#include <keyboard.h>
#include <stdint.h>

static void int_20();
static void int_21();

static void (*p[2])() = {int_20, int_21}; // Arreglo de punteros a funcion

void irqDispatcher(uint64_t irq) {
	(*p[irq])();
	return;
}

void int_20() {
	timer_handler();
}

void int_21() {
	keyboard_handler();
}
