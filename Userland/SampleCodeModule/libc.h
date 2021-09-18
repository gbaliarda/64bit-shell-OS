#include <stdint.h>

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

typedef struct cpuidRegisters{
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
} cpuidRegisters;

typedef struct cpuInformation {
  //EAX = 1
  // Processor Info EDX
  unsigned char mmx;
  unsigned char sse;
  unsigned char sse2;
  

  // Processor Info ECX
  unsigned char sse3;
  unsigned char pclmulqdq;
  unsigned char vmx;
  unsigned char smx;
  unsigned char fma;
  unsigned char sse41;
  unsigned char sse42;
  unsigned char aes;
  unsigned char avx;
  unsigned char f16c;

  //EAX = 7
  // Extended Features ECX
  unsigned char vaes;
  unsigned char vpclmulqdq;
  // Extended Features EBX
  unsigned char avx2;
} cpuInformation;

void sys_write(unsigned int fd, const char *buffer, unsigned int count);

int sys_read(char *buffer, int limit, int *changed);

char sys_consoleInUse();

void printf(const char *str);


void printReg(const char *regName, uint64_t regValue);

void putChar(char c);

void printInt(unsigned int num);

int scanf(char *buffer, int *changed);

int getChar(char *buffer, int *changed);

int compareStrings(char *str1, char *str2);

void sys_inforeg();

void sys_printmem(uint64_t pointer);

void sys_getDateTime(Time *dateTime);

uint64_t atoi(char *str, int *ok);

uint64_t hex2int(char *hex, int *ok);

void throwInvalidOpcode();

void sys_clearScreen();

// Retorna 0 si las raices son imaginarias, 1 si son reales
int computeZeros(double a, double b, double c, double *root1, double *root2);

void divZero();

void dtos(double num, char *buff);

double strToDouble(char *str, int *ok);

int cpuid(cpuInformation *cpuidData);

void getCpuProcessorInfo(cpuidRegisters *cpuidRegister);

void getCpuExtendedFeatures(cpuidRegisters *cpuidRegister);

int cpuidAvailability();

int getCpuLeaf();
