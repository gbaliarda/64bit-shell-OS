#ifndef PIPES_H
#define PIPES_H

#include "../include/scheduler.h"

typedef struct fdPipe {
  char readable;
  char writable;
  struct pipe *pipe;
} fdPipe;

int createPipe(fdPipe *f0, fdPipe *f1);

int openPipeId(fdPipe *fd, uint32_t id, uint8_t reader);

fdPipe *createFdPipe();

void closeFdPipe(fdPipe *fd);

int pipeWrite(fdPipe *fd, char* string);

int pipeRead(fdPipe *fd, char* buffer);

void printPipes();

#endif