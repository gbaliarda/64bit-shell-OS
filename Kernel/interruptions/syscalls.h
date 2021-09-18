#ifndef SYSCALLS_H_
#define SYSCALLS_H_

typedef struct Time {
	int hour;
	int minutes;
	int seconds;
  int day;
  int month;
  int year;
} Time;

typedef struct Registers {
  uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
} Registers;

void saveState(Registers *registers);

void syscallHandler();

int64_t write(uint64_t fd, const char* buf, uint64_t count);

int read(char *buf, int limit, int *changed);

void inforeg();

void printmem(uint64_t pointer);

void getDateTime(Time *dayTime);
void getTimeRTC(Time *dayTime);

void clearScreen();

char consoleInUse();

#endif