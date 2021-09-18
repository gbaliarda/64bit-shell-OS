#include "../include/naiveConsole.h"
#include "keyboard.h"
#include "time.h"

#define ZERO_EXCEPTION_ID 0
#define OPCODE_EXCEPTION_ID 6

static void zero_division();
static void opcode_exception();

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
	else if (exception == OPCODE_EXCEPTION_ID)
		opcode_exception();
}

static void zero_division() {
	// Handler para manejar excepcíon
	ncPrint("Exception 0: Division by zero.\n");
}

static void opcode_exception(){
	ncPrint("Exception 6: Invalid opcode.\n");
}	

void rebootConsole() {
	ncPrint("Press any key to reboot this shell");
	ncNewline();
	while (1) {
		_hlt();
		if (ticks_elapsed() % 9 == 0)
			displayCursor();
			unsigned char key = getInput();
			if (key != 0)
				break;
	}
	ncClear();
	ncPrint("> ");
}