#ifndef BUILTIN_H
#define BUILTIN_H

#include "./libc.h"

void help(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void clear(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void mem(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void sem(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void kill(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void nice(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void block(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void pipe(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

void ps(char args[MAX_ARG_AMT+1][MAX_ARG_COMMAND_LEN+1]);

#endif 