#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
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

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{

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

void runShells() {
	((EntryPoint)sampleCodeModuleAddress)();
}

int main()
{	
	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncNewline();

	// Cargamos la IDT
	ncPrint("Loading IDT...");
	ncNewline();
	load_idt();

	ncPrint("Loading Shells...");
	wait(1); // 1 seg de espera 
	ncNewline();
	ncClear();

	initializeShells();

	runShells();

	return 0;
}
