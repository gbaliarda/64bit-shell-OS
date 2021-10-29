#include "./apps.h"
#include "./syscalls.h"
#include "./libc.h"

void cat(int argc, const char* argv[]) {
	char buffer[200];
	while(scanf(buffer) > 0) {
		printf(buffer);
		printf("\n");
	}
	sys_exit(); 
}

void wc(int argc, const char *argv[]) {
	
}

void filter(int argc, const char *argv[]) {
	char buffer[200];
  while(scanf(buffer) >= 0) {
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

void p1(int argc, const char* argv[]) {
	for(int z = 0; z < 1; z++) {
		printf("Cantidad de argumentos: "); printInt(argc);
		for (int i = 0; i < argc; i++) {
			printf(argv[i]);
			printf(" ");
		}
		printf("\n");
	}
	sys_exit();
}

void p2(int argc, const char* argv[]) {
	fdPipe *fd = sys_createFdPipe();

	sys_openPipeId(fd, 1, 0);

	sys_pipeWrite(fd, "Hola");
	sys_closeFdPipe(fd);
	sys_exit();
}

void p3(int argc, const char* argv[]) {
	fdPipe *fd = sys_createFdPipe();

	if (sys_openPipeId(fd, 1, 1) == -1) {
		sys_closeFdPipe(fd);
		sys_exit();
	}

	char buff[10];
	sys_pipeRead(fd, buff);
	sys_closeFdPipe(fd);

	printf(buff);
	printf("\n");
	sys_exit();
}

void p4(int argc, const char* argv[]) {
	Semaphore *sem = sys_semOpen(1, 0);
	printf("Soy p4 y hora de esperar!\n");
	sys_semWait(sem);
	printf("Soy p4 y me canse de esperar >:(\n");
	sys_exit();
}

void p5(int argc, const char* argv[]) {
	Semaphore *sem = sys_semOpen(1, 0);
	printf("Voy a despertar a p4!\n");
	sys_semPost(sem);
	printf("Desperte a p4! :D\n");
	sys_exit();
}
 

void loop(int argc, const char* argv[]) {
	while(1) {
		for(int i = 0; i < 100000000; i++);
		printf("H");
	}
}


// void cat(int argc, const char* argv[]) {
// 	printf("Cat\n");
// 	sys_exit();
// }

// void wc(int argc, const char* argv[]) {
// 	// printf("Running WC\n");
// 	char buffer[101];
// 	scanf(buffer);
// 	printf(buffer);
// 	printf("\n");
// 	sys_exit();
// }