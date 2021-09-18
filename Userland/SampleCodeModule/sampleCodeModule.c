#include "libc.h"

void printProcessorInfo(cpuInformation *cpuidData, int maxEaxValue);

void executeCommand(char * buffer) {
	int index = 0;
	char command[21];
	char args[5][21]; //Matriz de hasta 5 argumentos con 20 de longitud cada uno

	while (index < 20 && buffer[index] && buffer[index] != ' ') {
		command[index] = buffer[index];
		index++;
	}

	// Comando demasiado largo, no se encuentra
	if (index == 20 && buffer[index] != ' ') {
		printf("Command not found\n");
		return;
	}

	command[index] = 0;
	while (buffer[index] && buffer[index] == ' ') //Si tiene argumentos despues 
		index++; // Salteamos los espacios

	int argNum = 0;	
	while (argNum < 5 && buffer[index]) {
		int indexArg = 0;
		// Leemos argumento
		while (indexArg < 20 && buffer[index] && buffer[index] != ' '){
			args[argNum][indexArg] = buffer[index];
			indexArg++;
			index++;
		}
			
		args[argNum][indexArg] = 0; //0 final del argumento 
		argNum++;

		// Si el argumento era muy largo, se guarda una parte y el resto se descarta
		while (buffer[index] && buffer[index] != ' ')
			index++;
		
		// Avanzo al proximo argumento
		while (buffer[index] && buffer[index] == ' ')
			index++;
			
	}

	if (compareStrings(command, "help")) {
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
	else if (compareStrings(command, "inforeg")) {
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
	else if (compareStrings(command, "printmem")) {
		int ok = 1;
		uint64_t pointer = hex2int(args[0], &ok);
		if(ok)
			sys_printmem(pointer);
		else
			printf("Invalid address\n");
	} 
	else if (compareStrings(command, "printDateTime")) {
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
	else if (compareStrings(command, "zeroException")) {
		divZero();
	}
	else if (compareStrings(command, "opcodeException"))
		throwInvalidOpcode();
	else if (compareStrings(command, "clear"))
		sys_clearScreen();
	else if (compareStrings(command, "computeZeros")) {
		char res[20];
		int ok = 1;
		double a, b, c;
		a = strToDouble(args[0], &ok);
		b = strToDouble(args[1], &ok);
		c = strToDouble(args[2], &ok);
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
	else if (compareStrings(command, "cpuid")) {
		cpuInformation cpuidData;
		int maxEaxValue = cpuid(&cpuidData);
		printProcessorInfo(&cpuidData, maxEaxValue);
	} 
	else
		printf("Command not found, try 'help'\n");
	
}

int main() {

	char bufferLeft[101];
	char bufferRight[101];

	char *buffer;
	if (sys_consoleInUse())
		buffer = bufferRight;
	else
		buffer = bufferLeft;
	
	int changed = 1;

	while (1) {
		if (!changed)
			printf("> ");
		else
			changed = 0;
		scanf(buffer, &changed);

		if (changed) {
			if (buffer == bufferLeft)
				buffer = bufferRight;
			else
				buffer = bufferLeft;
			continue;
		}
		executeCommand(buffer);

	}
									
	return 0xDEADBEEF; 
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

