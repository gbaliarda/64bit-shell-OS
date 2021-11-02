#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include "./tests/tests.h"

#define MAX_ARG_AMT 5
#define MAX_ARG_COMMAND_LEN 20
#define NULL ((void *)0)

typedef struct Semaphore {
  uint32_t id;
  int value;
} Semaphore;

typedef struct fdPipe {
  char readable;
  char writable;
  struct pipe *pipe;
} fdPipe;

void printf(const char *str);

void printReg(const char *regName, uint64_t regValue);

void putChar(char c);

void printInt(unsigned int num);

int scanf(char *buffer);

int getChar(char *buffer);

int compareStrings(char *str1, char *str2);

uint64_t atoi(char *str, int *ok);

int itos(int value, char* target, int initialIndex);

uint64_t hex2int(char *hex, int *ok);

void dtos(double num, char *buff);

double strToDouble(char *str, int *ok);

char* strcpy(char* destination, const char* source);

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);

int createProcess(uint64_t ip, unsigned int argc, char argv[6][21], fdPipe *customStdin, fdPipe *customStdout);

#endif