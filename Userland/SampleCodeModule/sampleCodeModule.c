#include "./libc.h"

int main() {
	char buffer[101];

	while (1) {
		printf("> ");
		scanf(buffer);
		executeCommand(buffer);
	}						
	return 0; 
}
