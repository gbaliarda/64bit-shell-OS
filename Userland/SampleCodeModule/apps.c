// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./builtin.h"
#include "./apps.h"
#include "./syscalls.h"
#include "./libc.h"

#define PROCESS_QTY 10
#define BUILTIN_QTY 14

char *processList[PROCESS_QTY] = { "p1", "p2", "p3", "p4", "p5", "loop", "cat", "wc", "filter", "phylo" };
void (*processFunc[])(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) = { p1, p2, p3, p4, p5, loop, cat, wc, filter, philo };

char *builtinList[BUILTIN_QTY] = { "help", "clear", "mem", "sem", "kill", "nice", "block", "pipe", "ps", "testmm", "testprio", "testproc", "testsync", "testnosync" };
void (*builtinFunc[])(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) = { help, clear, mem, sem, kill, nice, block, pipe, ps, test_mm, test_prio, test_processes, test_sync, test_no_sync };

void loop(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	int ok = 1;
	int delay = atoi((char *)argv[1], &ok);
	if (!ok) {
		printf("Please enter a valid delay\n");
		sys_exit();
	}
	while(1) {
		printf("Hello World! My PID is: ");
		printInt(sys_getPid());
		putChar('\n');
		sys_sleep(delay);
	}
}

void cat(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	char buffer[200];
	while(scanf(buffer) > 0) {
		printf(buffer);
		printf("\n");
	}
	sys_exit(); 
}

void wc(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	char buffer[200];
	int n = 0;
	while (scanf(buffer) > 0)
		n++;

	printf("Cantidad de lineas leidas: ");
	printInt(n);
	printf("\n");
	sys_exit();
}

void filter(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	char buffer[200];
  while(scanf(buffer) > 0) {
    int i = 0;
		while (buffer[i]) {
			switch (buffer[i]) {
				case 'a':
				case 'e':
				case 'i':
				case 'o':
				case 'u':
				case 'A':
				case 'E':
				case 'I':
				case 'O':
				case 'U':
					break;
				default:
					putChar(buffer[i]);
			}
			i++;
		}
    printf("\n");
	}
	sys_exit();
}

void p1(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	printf("Cantidad de argumentos: "); printInt(argc); printf("\n");
	for (int i = 0; i < argc; i++) {
		printf(argv[i]);
		printf(" ");
	}
	printf("\n");
	sys_exit();
}

void p2(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	fdPipe *fd = sys_createFdPipe();

	sys_openPipeId(fd, 1, 0);

	sys_pipeWrite(fd, "Soy p2!");
	sys_closeFdPipe(fd);
	sys_exit();
}

void p3(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	fdPipe *fd = sys_createFdPipe();

	if (sys_openPipeId(fd, 1, 1) == -1) {
		sys_closeFdPipe(fd);
		sys_exit();
	}

	sys_sleep(5); // wait for p2 to leave msg in pipe

	char buff[10];
	int n = sys_pipeRead(fd, buff);
	sys_closeFdPipe(fd);

	if(n > 0) {
		printf("Mensaje de P2: ");
		printf(buff);
		printf("\n");
	}
	sys_exit();
}

void p4(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	Semaphore *sem = sys_semOpen(1, 0);
	printf("Soy p4 y hora de esperar!\n");
	sys_semWait(sem);
	printf("Soy p4 y me desperte!\n");
	sys_exit();
}

void p5(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	Semaphore *sem = sys_semOpen(1, 0);
	printf("Voy a despertar a p4!\n");
	sys_semPost(sem);
	sys_exit();
}

static void executeCommand(char * buffer);

void shell(int argc, const char argv[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]) {
	char buffer[101];

	while (1) {
		printf("> ");
		scanf(buffer);
		executeCommand(buffer);
	}
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

	for (int i = 0; i < PROCESS_QTY; i++) {
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

static void executeCommand(char * buffer) {
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

	index = 1;
	while (index < argNum - 1) {
		if (compareStrings(args[index], "|")) {
			managePipe(args, index, argNum);
			return;
		}
		index++;
	}

	index = -1;
	for (int i = 0; i < PROCESS_QTY; i++) {
		if (compareStrings(args[0], processList[i]))
			index = i;
	}
	if (index >= 0) {
		createProcess((uint64_t)processFunc[index], argNum, args, NULL, NULL);
		return;
	}

	for (int i = 0; i < BUILTIN_QTY; i++) {
		if (compareStrings(args[0], builtinList[i]))
			index = i;
	}

	if (index >= 0)
		builtinFunc[index](args);
	else
		printf("Command not found, try 'help'\n");

}
