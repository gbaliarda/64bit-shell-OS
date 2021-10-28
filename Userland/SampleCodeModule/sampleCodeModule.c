#include "libc.h"

#define MAX_ARG_AMT 5
#define MAX_ARG_COMMAND_LEN 20

void printProcessorInfo(cpuInformation *cpuidData, int maxEaxValue);

void p1(int argc, char **argv) {
	for(int z = 0; z < 100; z++) {
		for (int j = 0; j < 10000000; j++);
		for (int i = 0; i < 10000000; i++);
			printf("Cantidad de argumentos: "); printInt(argc);
			for (int i = 0; i < argc; i++) {
				printf(argv[i]);
				printf(" ");
			}
			printf("\n");
	}
	sys_exit();
}

void p2() {
	fdPipe *fd = sys_createFdPipe();

	sys_openPipeId(fd, 1, 0);

	sys_pipeWrite(fd, "Hola");
	sys_closeFdPipe(fd);
	sys_exit();
}

void p3() {
	fdPipe *fd = sys_createFdPipe();

	if (sys_openPipeId(fd, 1, 1) == -1)
		sys_exit();

	char buff[10];
	sys_pipeRead(fd, buff);
	sys_closeFdPipe(fd);

	printf(buff);
	printf("\n");
	sys_exit();
}
 

void loop(int segundos) {
	while(1) {
		for(int i = 0; i < 100000000; i++);
		printf("H");
	}
}

void loop2(int segundos) {
	while(1) {
		for(int i = 0; i < 100000000; i++);
		printf("C");
	}
}

void executeCommand(char * buffer) {
	int index = 0;
	char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1];

	while (index < MAX_ARG_COMMAND_LEN && buffer[index] && buffer[index] != ' ') {
		args[0][index] = buffer[index];
		index++;
	}

	if (index == MAX_ARG_COMMAND_LEN && buffer[index] != ' ') {
		printf("Command not found, try 'help'\n");
		return;
	}

	args[0][index] = 0;
	while (buffer[index] && buffer[index] == ' ') 
		index++;

	unsigned int argNum = 1;	
	while (argNum < MAX_ARG_AMT+1 && buffer[index]) {
		int indexArg = 0;

		while (indexArg < MAX_ARG_COMMAND_LEN && buffer[index] && buffer[index] != ' '){
			args[argNum][indexArg] = buffer[index];
			indexArg++;
			index++;
		}
			
		args[argNum][indexArg] = 0;
		argNum++;

		while (buffer[index] && buffer[index] != ' ')
			index++;
		
		while (buffer[index] && buffer[index] == ' ')
			index++;	
	}


	if (compareStrings(args[0], "help")) {
		printf("-------COMMANDS------\n");
		printf("1: help\n");
		printf("2: inforeg\n");
		printf("3: printmem\n");
		printf("4: printDateTime\n");
		printf("5: zeroException\n");
		printf("6: opcodeException\n");
		printf("7: clear\n");
		printf("8: computeZeros\n");
		printf("9: cpuid\n");
	} 
	else if (compareStrings(args[0], "inforeg")) {
		Registers registers;
		sys_inforeg(&registers);
		printReg("rax",registers.rax);
		printReg("rbx",registers.rbx);
		printReg("rcx",registers.rcx);
		printReg("rdx",registers.rdx);
		printReg("rbp",registers.rbp);
		printReg("rdi",registers.rdi);
		printReg("rsi",registers.rsi);
		printReg("r8",registers.r8);
		printReg("r9",registers.r9);	
		printReg("r10",registers.r10);
		printReg("r11",registers.r11);
		printReg("r12",registers.r12);
		printReg("r13",registers.r13);
		printReg("r14",registers.r14);
		printReg("r15",registers.r15);
	}
	else if (compareStrings(args[0], "printmem")) {
		int ok = 1;
		uint64_t pointer = hex2int(args[1], &ok);
		if(ok)
			sys_printmem(pointer);
		else
			printf("Invalid address\n");
	} 
	else if (compareStrings(args[0], "printDateTime")) {
		Time dateTime;
		sys_getDateTime(&dateTime);
		printInt(dateTime.day);
		putChar('\\');
		printInt(dateTime.month);
		putChar('\\');
		printInt(dateTime.year);
		putChar(' ');
		printInt(dateTime.hour);
		putChar(':');
		printInt(dateTime.minutes);
		putChar(':');
		printInt(dateTime.seconds);
		putChar('\n');
	} 
	else if (compareStrings(args[0], "zeroException")) {
		divZero();
	}
	else if (compareStrings(args[0], "opcodeException"))
		throwInvalidOpcode();
	else if (compareStrings(args[0], "clear"))
		sys_clearScreen();
	else if (compareStrings(args[0], "computeZeros")) {
		char res[20];
		int ok = 1;
		double a, b, c;
		a = strToDouble(args[1], &ok);
		b = strToDouble(args[2], &ok);
		c = strToDouble(args[3], &ok);
		if (!ok) {
			printf("Invalid arguments a, b or c\n");
			return;
		}
		double root1;
		double root2;
		if (computeZeros(a, b, c, &root1, &root2)) {
			dtos(root1, res);
			printf("Root 1: ");
			printf(res);
			putChar('\n');
			dtos(root2, res);
			printf("Root 2: ");
			printf(res);
			putChar('\n');
		} 	
		else
			printf("Roots are not real numbers\n");
	}
	else if (compareStrings(args[0], "cpuid")) {
		cpuInformation cpuidData;
		int maxEaxValue = cpuid(&cpuidData);
		printProcessorInfo(&cpuidData, maxEaxValue);
	}
	else if (compareStrings(args[0], "p1"))
		sys_createProcess((uint64_t)&p1, 1024, 10, argNum, (char **)args);
	else if(compareStrings(args[0], "loop")) {
		sys_createProcess((uint64_t)&loop, 1024, 2, argNum, (char **)args);
	} else if(compareStrings(args[0], "loop2")) {
		sys_createProcess((uint64_t)&loop2, 1024, 2, argNum, (char **)args);
	} else if(compareStrings(args[0], "ps")) {
		sys_printProcess();
	} else if(compareStrings(args[0], "kill")) {
		int ok = 1;
		sys_killProcess((uint32_t) atoi(args[1], &ok));
	} else if(compareStrings(args[0], "nice")) {
		int ok = 1;
		sys_changePriority((uint32_t) atoi(args[1], &ok), (uint8_t) atoi(args[2], &ok));
	} else if(compareStrings(args[0], "block")) {
		int ok = 1;
		sys_changeState((uint32_t) atoi(args[1], &ok));
	} else if(compareStrings(args[0], "p2"))
		sys_createProcess((uint64_t)&p2, 1024, 2, argNum, (char **)args);
	else if(compareStrings(args[0], "p3"))
		sys_createProcess((uint64_t)&p3, 1024, 2, argNum, (char **)args);
	else if(compareStrings(args[0], "psem"))
		sys_printSemaphores();
	else if(compareStrings(args[0], "pipe"))
		sys_printPipes();
	else
		printf("Command not found, try 'help'\n");

}

void printProcessorInfo(cpuInformation *cpuidData, int maxEaxValue) {
	if(maxEaxValue == -1) {
		printf("cpuid not supported.\n");
		return;
	} 
	else if (maxEaxValue < 1) {
		printf("Processor Info not supported.\n");
		return;
	}
	
	printf("mmx_support: ");
	cpuidData->mmx ? printf("Yes") : printf("No");
	putChar('\n');

	printf("sse_support: ");
	cpuidData->sse ? printf("Yes") : printf("No");
	putChar('\n');

	printf("sse2_support: ");
	cpuidData->sse2 ? printf("Yes") : printf("No");
	putChar('\n');

	printf("sse3_support: ");
	cpuidData->sse3 ? printf("Yes") : printf("No");
	putChar('\n');

	printf("pclmulqdq_support: ");
	cpuidData->pclmulqdq ? printf("Yes") : printf("No");
	putChar('\n');

	printf("vmx_support: ");
	cpuidData->vmx ? printf("Yes") : printf("No");
	putChar('\n');

	printf("smx_support: ");
	cpuidData->smx ? printf("Yes") : printf("No");
	putChar('\n');

	printf("fma_support: ");
	cpuidData->fma ? printf("Yes") : printf("No");
	putChar('\n');

	printf("sse4.1_support: ");
	cpuidData->sse41 ? printf("Yes") : printf("No");
	putChar('\n');

	printf("sse4.2_support: ");
	cpuidData->sse42 ? printf("Yes") : printf("No");
	putChar('\n');

	printf("aes_support: ");
	cpuidData->aes ? printf("Yes") : printf("No");
	putChar('\n');

	printf("avx_support: ");
	cpuidData->avx ? printf("Yes") : printf("No");
	putChar('\n');

	printf("f16c_support: ");
	cpuidData->f16c ? printf("Yes") : printf("No");
	putChar('\n');

	if(maxEaxValue < 7) {
		printf("Extended Features not supported.\n");
		return;
	}

	printf("vaes_support: ");
	cpuidData->vaes ? printf("Yes") : printf("No");
	putChar('\n');

	printf("vpclmulqdq_support: ");
	cpuidData->vpclmulqdq ? printf("Yes") : printf("No");
	putChar('\n');

	printf("avx2_support: ");
	cpuidData->avx2 ? printf("Yes") : printf("No");
	putChar('\n');
}

int main() {

	char buffer[101];

	while (1) {
		printf("> ");
		scanf(buffer);
		executeCommand(buffer);
	}
									
	return 0xDEADBEEF; 
}



