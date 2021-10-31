#include "./libc.h"
#include "./syscalls.h"
#include "./apps.h"

char *processList[5] = { "loop", "cat", "wc", "filter", "phylo" };
void (*processFunc[])(int argc, const char argv[6][21]) = { loop, cat, wc, filter };

static int strlen(const char *str) {
  int len = 0;
  while (str[len] != 0)
    len++;
  return len;
}

void printf(const char *str) {
  sys_write(str, strlen(str));
}

void putChar(char c) {
  sys_write(&c, 1);
}

int scanf(char *buffer) {
  return sys_read(buffer, -1);
}

int getChar(char *buffer) {
  return sys_read(buffer, 1);
}

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	*p = 0;

	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void printInt(unsigned int num) {
  char buffer[10];
  int digits = uintToBase(num, buffer, 10);
  sys_write(buffer, digits);
}

void printReg(const char *regName, uint64_t regValue) {
  char buffer[17];
	printf(regName);
	printf(": ");
	int digits = uintToBase(regValue, buffer, 16);
	for (int i = 1; i < 16 - digits; i++)
		printf("0");
	printf(buffer);
	putChar('\n');
}

int compareStrings(char *str1, char *str2) {
  int index = 0;
  
  while(str1[index] && str2[index]){
    if(str1[index] != str2[index])
      return 0;
    index++;
  }
  
  if(str1[index] || str2[index])
    return 0;
  return 1;
}

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 */
uint64_t hex2int(char *hex, int *ok) {
    uint64_t val = 0;
    while (*hex) {
        uint8_t byte = *hex++;
        
        if (!(byte >= '0' && byte <= '9') && !(byte >= 'a' && byte <= 'f') && !(byte >= 'A' && byte <= 'F')) {
          *ok = 0;
          return 0;
        }
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

uint64_t atoi(char *str, int *ok){
  uint64_t num = 0;
  
  for (int i = 0; str[i]; ++i) {
    if(str[i] < '0' || str[i] > '9') {
      *ok = 0;
      return 0;
    }
    else
      num = num * 10 + str[i] - '0';
  }
  return num;
}

int itos(int value, char* target, int initialIndex){
    int digit; 
    int sign = 1; 
    int i = -1, j = initialIndex;
    char aux[11];
    if(value < 0){
        sign = 0;
        value *= -1;
    }
    do {
      i++;
      digit = value % 10;
      aux[i] = digit + '0'; // 48 = '0' 
      value /= 10; 
    } while(value > 0);
    if (!sign)
      target[j++] = '-';
    while(i > -1)
      target[j++] = aux[i--];
    target[j] = 0;
    return j;
}

void dtos(double num, char *buff) {
  int len = itos((int)num, buff, 0);

  num -= (int)num;
  buff[len++] = ',';

  num *= 1000000000;
  if (num < 0)
    num *= -1;

  itos((int)num, buff, len);
  return;
}

double strToDouble(char *str, int *ok) {
  int sign = 1, idx = 0, auxIdx = 0;
  if (str[0] == '-') {
    sign = -1;
    str++;
  }
  
  char aux[10];
  while (str[idx] && str[idx] != '.') {
    aux[idx] = str[idx];
    idx++;
  }

  aux[idx] = 0;
  int integerPart = atoi(aux, ok);

  if (!(*ok))
    return -1;

  double decimalPart = 0;

  if (str[idx] == '.') {
    str++;
    while (str[idx])
      aux[auxIdx++] = str[idx++];
    aux[auxIdx] = 0;
    decimalPart = (double) atoi(aux, ok);
  }
  auxIdx = 0;
  while (aux[auxIdx] && aux[auxIdx] == '0')
    auxIdx++;

  if (!(*ok))
    return -1;

  while (decimalPart >= 1)
    decimalPart /= 10;

  while (auxIdx > 0) {
    decimalPart /= 10;
    auxIdx--;
  }

  return ((double) integerPart + decimalPart) * sign;
}

int cpuid(cpuInformation* cpuidData) {
  if(cpuidAvailability() == 0)
    return -1;
  
  cpuidRegisters cpuidRegister;
  
  int maxEaxValue = getCpuLeaf();

  if(maxEaxValue < 1)
    return maxEaxValue;

  getCpuProcessorInfo(&cpuidRegister);
  
  cpuidData->mmx = (cpuidRegister.edx & 0x00800000) >> 23;
  cpuidData->sse = (cpuidRegister.edx & 0x02000000) >> 25;
  cpuidData->sse2 = (cpuidRegister.edx & 0x04000000) >> 26;

  cpuidData->sse3 = (cpuidRegister.ecx & 0x00000001);
  cpuidData->pclmulqdq = (cpuidRegister.ecx & 0x00000002) >> 1;
  cpuidData->vmx = (cpuidRegister.ecx & 0x00000020) >> 5;
  cpuidData->smx = (cpuidRegister.ecx & 0x00000040) >> 6;
  cpuidData->fma = (cpuidRegister.ecx & 0x00001000) >> 12;
  cpuidData->sse41 = (cpuidRegister.ecx & 0x00080000) >> 19;
  cpuidData->sse42 = (cpuidRegister.ecx & 0x00100000) >> 20;
  cpuidData->aes = (cpuidRegister.ecx & 0x02000000) >> 25;
  cpuidData->avx = (cpuidRegister.ecx & 0x10000000) >> 28;
  cpuidData->f16c = (cpuidRegister.ecx & 0x20000000) >> 29;

  if(maxEaxValue < 7)
    return maxEaxValue;

  getCpuExtendedFeatures(&cpuidRegister);

  cpuidData->vaes = (cpuidRegister.ecx & 0x00000200) >> 9;
  cpuidData->vpclmulqdq = (cpuidRegister.ecx & 0x00000400) >> 10;

  cpuidData->avx2 = (cpuidRegister.ebx & 0x00000020) >> 5;

  return maxEaxValue;
}

char* strcpy(char* destination, const char* source) {
  if (destination == NULL)
    return NULL;

  char *ptr = destination;

  while (*source != '\0') {
    *destination = *source;
    destination++;
    source++;
  }

  *destination = '\0';
  return ptr;
}

int createProcess(uint64_t ip, unsigned int argc, char argv[6][21], fdPipe *customStdin, fdPipe *customStdout) {
  uint8_t priority = 3;
	if (customStdin == NULL) {
		priority = (compareStrings("&", argv[argc-1]) ? 3 : 1);
		if (priority == 3)
    	argc--;
	}

  char *args = (char *)sys_alloc(argc*21);
  int argsIndex = 0;
  for (int i = 0; i < argc; i++) {
    int j = 0;
    while (argv[i][j])
      args[argsIndex++] = argv[i][j++];
    args[argsIndex++] = 0;
  }

  int aux = sys_createProcess(ip, priority, argc, args, customStdin, customStdout);
  sys_free(args);
	return aux;
}

static void printProcessorInfo(cpuInformation *cpuidData, int maxEaxValue) {
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

static void managePipe(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1], int index, int argNum) {
	int indexP1 = -1, indexP2 = -1;
	char *p1 = args[0];
	char *p2 = args[index + 1];
	fdPipe *pipeRead = sys_createFdPipe();
	fdPipe *pipeWrite = sys_createFdPipe();
	sys_createPipe(pipeRead, pipeWrite);

	char args1[MAX_ARG_AMT][MAX_ARG_COMMAND_LEN+1];
	char args2[MAX_ARG_AMT][MAX_ARG_COMMAND_LEN+1];
	for (int i = 0; i < index; i++) {
		strcpy(args1[i], args[i]);
	}
	for (int i = 0, j = index+1; j < argNum; i++)
		strcpy(args2[i], args[j++]);

	for (int i = 0; i < 5; i++) {
		if (compareStrings(p1, processList[i]))
			indexP1 = i;
		if (compareStrings(p2, processList[i]))
			indexP2 = i;
	}

	if (indexP1 == -1 || indexP2 == -1) {
		printf("Command not found, try 'help'\n");
		return;
	}

	createProcess((uint64_t)processFunc[indexP1], index, args1, NULL, pipeWrite);
	createProcess((uint64_t)processFunc[indexP2], argNum-index-1, args2, pipeRead, NULL);
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

	// PIPES
	index = 1;
	while (index < argNum - 1) {
		if (compareStrings(args[index], "|")) {
			managePipe(args, index, argNum);
			return;
		}
		index++;
	}

	if (compareStrings(args[0], "help")) {
		printf("-------COMMANDS------\n");
		printf("1: help\n");
		printf("2: inforeg\n");
		printf("3: printmem\n");
		printf("4: printDateTime\n");
		printf("5: clear\n");
		printf("6: computeZeros\n");
		printf("7: cpuid\n");
		printf("8: mem\n");
		printf("9: sem\n");
		printf("10: kill\n");
		printf("11: nice\n");
		printf("12: block\n");
		printf("13: loop\n");
		printf("14: pipe\n");
		printf("15: ps\n");
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
	} else if (compareStrings(args[0], "clear"))
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
		createProcess((uint64_t)&p1, argNum, args, NULL, NULL);
	else if (compareStrings(args[0], "loop")) {
		createProcess((uint64_t)&loop, argNum, args, NULL, NULL);
	} else if (compareStrings(args[0], "ps")) {
		sys_printProcess();
	} else if (compareStrings(args[0], "kill")) {
		int ok = 1;
		sys_killProcess((uint32_t) atoi(args[1], &ok));
	} else if (compareStrings(args[0], "nice")) {
		int ok = 1;
		sys_changePriority((uint32_t) atoi(args[1], &ok), (uint8_t) atoi(args[2], &ok));
	} else if (compareStrings(args[0], "block")) {
		int ok = 1;
		sys_changeState((uint32_t) atoi(args[1], &ok));
	} else if (compareStrings(args[0], "p2"))
		createProcess((uint64_t)&p2, argNum, args, NULL, NULL);
	else if (compareStrings(args[0], "p3"))
		createProcess((uint64_t)&p3, argNum, args, NULL, NULL);
	else if (compareStrings(args[0], "p4"))
		createProcess((uint64_t)&p4, argNum, args, NULL, NULL);
	else if (compareStrings(args[0], "p5"))
		createProcess((uint64_t)&p5, argNum, args, NULL, NULL);
	else if (compareStrings(args[0], "sem"))
		sys_printSemaphores();
	else if (compareStrings(args[0], "pipe"))
		sys_printPipes();
	else if (compareStrings(args[0], "cat")) {
		createProcess((uint64_t)&cat, argNum, args, NULL, NULL);
	}
	else if (compareStrings(args[0], "filter")) {
		createProcess((uint64_t)&filter, argNum, args, NULL, NULL);
	}
	else if (compareStrings(args[0], "phylo")) {
		createProcess((uint64_t)&philo, argNum, args, NULL, NULL);
	}
	else if (compareStrings(args[0], "wc")) {
		createProcess((uint64_t)&wc, argNum, args, NULL, NULL);
	}
	else if (compareStrings(args[0], "mem")) {
		unsigned int mem[3];
		sys_memStatus(mem);
		printf("Heap size: ");
		printInt(mem[0]);
		printf(" bytes\n");
		printf("Heap left: ");
		printInt(mem[1]);
		printf(" bytes\n");
		printf("Heap used: ");
		printInt(mem[2]);
		printf(" bytes\n");
	}
	else
		printf("Command not found, try 'help'\n");

}