// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/naiveConsole.h"
#include "keyboard.h"
#include "interrupts.h"
#include "syscalls.h"
#include "time.h"
#include "../include/pipes.h"
#include "../include/scheduler.h"
#include "../include/lib.h"

int mayus = 0;
Registers b, bAux;
Registers *backupRegisters = &b;
Registers *backupAuxRegisters = &bAux;

void loadRegisters(Registers* registers, Registers *from);

int64_t write(char* buf, uint64_t count) {
  fdPipe *stdout = getCurrentStdout();
  if (!stdout) {
    for (int i = 0; i < count; i++)
      ncPrintChar(buf[i]);
  } else {
    pipeWrite(stdout, buf);
  }
  return count;
}

int read(char* buf, int limit) {
  int count = 0;
  unsigned char key;
  fdPipe *stdin = getCurrentStdin();
  pcb *currentProcess = getCurrentProcess();
  if (currentProcess->priority != 1 && stdin == NULL) {
    buf[0] = 0;
    return 0;
  }

  while (count < limit || limit == -1) {
    if (!stdin) {
      waitForKeyboard();
      key = getInput();
    } else {
      key = (unsigned char) pipeRead(stdin, NULL, 1);
    }

    switch (key) {
      case (unsigned char)-1:
        return -1;
      case 0:
        if (!stdin)
          continue;
        else {
          buf[count] = 0;
          return count;
        }

      case '\n':
        if (!stdin)
          ncNewline();
        buf[count] = 0;
        return count;

      case 8:
        if (count > 0 && ncBackspace())
          count--;
        break;

      // F1, guarda el estado de los registros en el momento actual
      case 17:
        loadRegisters(backupRegisters, backupAuxRegisters);
        break;

      case 18: // F2
        exit();
        break;

      case 19: // F3
        return -1;
        break;

      // shifts izq, der y sus release; y bloq mayus
      case 11:
      case 14:
      case 15:
      case 0xAA:
      case 0xB6:
        mayus = !mayus;
        break;
      
      default: 
        if(mayus && key >= 'a' && key <= 'z')
          key -= 'a' - 'A';
        if(mayus && key == '6')
          key = '&';
        if (count < 100)
          buf[count] = key;
        count++;
        if (!stdin)
          ncPrintChar(key);
        break;
    }
	}
  buf[count] = 0;
  return (count >= 100) ? 100 : count;
}

void loadRegisters(Registers* registers, Registers *from) {
  registers->rax = from->rax;
	registers->rbx = from->rbx;
	registers->rcx = from->rcx;
	registers->rdx = from->rdx;
	registers->rbp = from->rbp;
	registers->rdi = from->rdi;
	registers->rsi = from->rsi;
	registers->r8 = from->r8;
	registers->r9 = from->r9;
	registers->r10 = from->r10;
	registers->r11 = from->r11;
	registers->r12 = from->r12;
	registers->r13 = from->r13;
	registers->r14 = from->r14;
	registers->r15 = from->r15;
}

void inforeg(Registers *registers) {
  loadRegisters(registers, backupRegisters);
}

void saveBackup() {
  saveState(backupAuxRegisters);
}

void printmem(uint64_t pointer) {
  uint8_t *arr = (uint8_t*) pointer;
  for (int i = 0; i < 32; i++){
    if(arr[i] <= 0xF)
      ncPrint("0");
    ncPrintHex(arr[i]);
    ncPrint(" ");
  }
  ncNewline();
}

void getDateTime(Time *dayTime) {
  getTimeRTC(dayTime);
}

void clearScreen() {
  ncClear();
}

void exit() {
  closeFdPipe(getCurrentStdin());
  closeFdPipe(getCurrentStdout());
  exitCurrentProcess();
  runScheduler();
}

void printProcess() {
  printProcessList();
}

void killProcess(uint32_t pid) {
  killPid(pid);
}

void changePriority(uint32_t pid, uint8_t newPriority) {
  changeProcessPriority(pid, newPriority);
}

void changeState(uint32_t pid) {
  changeProcessState(pid);
}