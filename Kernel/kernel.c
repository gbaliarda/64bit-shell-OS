#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "./include/mm.h"
// Interrupts
#include <keyboard.h>
#include <interrupts.h>
#include <time.h>
#include <idtLoader.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const heapModuleAddress = (void*)0x600000;
static void * const memoryManagerAddress = (void*)0x50000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {

	ncPrint("[x64BareBones]");
	ncNewline();
	ncNewline();

	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};
	
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	ncNewline();

	return getStackBase();
}

void runShell() {
	((EntryPoint)sampleCodeModuleAddress)();
}

int main() {	
	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncNewline();

	ncPrint("Loading IDT...");
	ncNewline();
	load_idt();

	ncPrint("Loading Shell...");
	wait(1);
	ncNewline();
	ncClear();

	initMemManager(memoryManagerAddress, heapModuleAddress);

	void *ptr1 = alloc(100);
	void *ptr2 = alloc(200);
	void *ptr3 = alloc(300);

	memset(ptr1, 1, 100);
	memset(ptr2, 2, 200);
	memset(ptr3, 3, 300);

	ncPrintHex((uint64_t) ptr1);
	ncNewline();
	ncPrintHex((uint64_t) ptr2);
	ncNewline();
	ncPrintHex((uint64_t) ptr3);
	ncNewline();

	runShell();

	return 0;
}
