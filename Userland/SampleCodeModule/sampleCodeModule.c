#include "./include/libc.h"
#include "./include/syscalls.h"
#include "./include/apps.h"

#define MAX_ARG_AMT 5
#define MAX_ARG_COMMAND_LEN 20

int main() {
	char buffer[101];

	while (1) {
		printf("> ");
		scanf(buffer);
		executeCommand(buffer);
	}
									
	return 0xDEADBEEF; 
}



