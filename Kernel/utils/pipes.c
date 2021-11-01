#include "../include/pipes.h"
#include "../include/naiveConsole.h"

#define MAX_PIPES 20
#define PIPE_SIZE 512

typedef struct pipe {
  uint32_t id;
  char data[PIPE_SIZE];
  uint32_t readPos;
  uint32_t writePos;
  uint8_t readOpen;
  uint8_t writeOpen;
  uint32_t bytesToRead;
  pcb *waitingProcess;
} pipe;

pipe *pipes[MAX_PIPES];
uint8_t pipeAmount = 0;

int createPipe(fdPipe *f0, fdPipe *f1) {
  if (pipeAmount == MAX_PIPES)
    return -1;
    
  pipes[pipeAmount] = (pipe *) alloc(sizeof(pipe));
  pipes[pipeAmount]->readPos = 0;
  pipes[pipeAmount]->writePos = 0;
  pipes[pipeAmount]->readOpen = 1;
  pipes[pipeAmount]->writeOpen = 1;
  pipes[pipeAmount]->bytesToRead = 0;
  pipes[pipeAmount]->id = 0;
  pipes[pipeAmount]->waitingProcess = NULL;
  f0->pipe = pipes[pipeAmount];
  f0->readable = 1;
  f0->writable = 0;
  f1->pipe = pipes[pipeAmount];
  f1->readable = 0;
  f1->writable = 1;
  pipeAmount++;
  return 0;
}

int openPipeId(fdPipe *fd, uint32_t id, uint8_t reader) {
  if (id == 0)
    return -1;

  int i = 0;
  while (i < pipeAmount && pipes[i]->id != id)
    i++;

  fd->readable = reader ? 1 : 0;
  fd->writable = reader ? 0 : 1;
  
  if (i == pipeAmount) {
    if(pipeAmount == PIPE_SIZE)
        return -1;
    
    pipes[pipeAmount] = (pipe *) alloc(sizeof(pipe));
    fd->pipe = pipes[pipeAmount++];
    fd->pipe->bytesToRead = 0;
    fd->pipe->readPos = 0;
    fd->pipe->writePos = 0;
    fd->pipe->readOpen = reader ? 1 : 0;
    fd->pipe->writeOpen = reader ? 0 : 1;
    fd->pipe->id = id;
    return 0;
  }

  if ((reader && pipes[i]->readOpen) || (!reader && pipes[i]->writeOpen))
    return -1;
  if (reader)
    fd->pipe->readOpen = 1;
  else
    fd->pipe->writeOpen = 1;
  fd->pipe = pipes[i];
  return 1;
}

fdPipe *createFdPipe() {
  return (fdPipe *) alloc(sizeof(fdPipe));
}

static void deletePipeFromArray(pipe *p) {
  int i = 0;
  while(i < pipeAmount && pipes[i]->id != p->id)
    i++;
    
  if (i == pipeAmount)
    return;
  
  while( i < pipeAmount-1)
    pipes[i] = pipes[i+1];
  pipeAmount--;
}

void closeFdPipe(fdPipe *fd) {
  if (fd == NULL)
    return;
  

  if (fd->readable) {
    fd->pipe->readOpen = 0;
    if (!fd->pipe->writeOpen) {
      deletePipeFromArray(fd->pipe);
      free(fd->pipe);
    } else if (fd->pipe->waitingProcess != NULL) {
        fd->pipe->waitingProcess->pstate = 1;
        fd->pipe->waitingProcess = NULL;
    }
  } else if (fd->writable) {
    fd->pipe->writeOpen = 0;
    if (!fd->pipe->readOpen) {
      deletePipeFromArray(fd->pipe);
      free(fd->pipe);
    } else if (fd->pipe->waitingProcess != NULL) {
        fd->pipe->waitingProcess->pstate = 1;
        fd->pipe->waitingProcess = NULL;
    }
  }

  free(fd);
}

int pipeWrite(fdPipe *fd, char *string) {
  if (!fd->writable || (!fd->pipe->readOpen && fd->pipe->bytesToRead == PIPE_SIZE))
    return -1;

  int i = 0;
  while (string[i]) {
    if (fd->pipe->bytesToRead == PIPE_SIZE) {
      fd->pipe->waitingProcess = blockCurrentProcess();
      runScheduler();
    }

    fd->pipe->data[fd->pipe->writePos++] = string[i++];
    if (fd->pipe->writePos == PIPE_SIZE)
      fd->pipe->writePos = 0;
    fd->pipe->bytesToRead++;
  }
  if (fd->pipe->waitingProcess != NULL) {
    fd->pipe->waitingProcess->pstate = 1;
    fd->pipe->waitingProcess = NULL;
  }
  return i;
}

int pipeRead(fdPipe *fd, char *buffer, int limit) {
  ncPrint("");
  if (!fd->readable)
    return -1;

  if (fd->pipe->bytesToRead == 0) {
    if (!fd->pipe->writeOpen)
      return 0;
    fd->pipe->waitingProcess = blockCurrentProcess();
    runScheduler();
  }

  int i = 0;
  while (fd->pipe->bytesToRead > 0) {
    fd->pipe->bytesToRead--;
    if (fd->pipe->readPos == PIPE_SIZE)
      fd->pipe->readPos = 0;
    if (limit == 1) {
      i = (int) fd->pipe->data[fd->pipe->readPos++];
      break;
    }
    else
      buffer[i++] = fd->pipe->data[fd->pipe->readPos++];
  }

  if (fd->pipe->waitingProcess != NULL) {
    fd->pipe->waitingProcess->pstate = 1;
    fd->pipe->waitingProcess = NULL;
  }
  return i;
}

void printPipes() {
  uint32_t index = 0;
  ncPrint("ID      Bytes To Read       Blocked Process\n");
  while(index < pipeAmount) {
    ncPrintDec(pipes[index]->id);
    ncPrint("            ");
    ncPrintDec(pipes[index]->bytesToRead);
    ncPrint("                   ");
    if (pipes[index]->waitingProcess != NULL) {
      ncPrintDec(pipes[index]->waitingProcess->pid);
    } else {
      ncPrint("None");
    }
    ncPrint("\n");
    index++;
  }
}

void deleteProcessFromPipes(uint32_t pid) {
  for (int i = 0; i < pipeAmount; i++)
    if (pipes[i]->waitingProcess->pid == pid)
      pipes[i]->waitingProcess = NULL;
}