#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

void ncPrint(const char * string);
int ncStrlen(const char *str);
void ncPrintChar(char character);
void ncNewline();
void ncPrintDec(int64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncPrintReg(const char *regName, uint64_t regValue);
void ncClear();
int ncBackspace();
void displayCursor();
void deleteCursor();

#endif