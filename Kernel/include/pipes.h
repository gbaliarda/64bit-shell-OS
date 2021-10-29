#ifndef PIPES_H
#define PIPES_H

#include "../include/scheduler.h"

int createPipe(fdPipe *f0, fdPipe *f1);

int openPipeId(fdPipe *fd, uint32_t id, uint8_t reader);

fdPipe *createFdPipe();

void closeFdPipe(fdPipe *fd);

int pipeWrite(fdPipe *fd, char* string);

int pipeRead(fdPipe *fd, char* buffer, int limit);

void printPipes();

#endif