#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>

#define MAX_ARGS        1024
#define MAX_HISTORY     10
#define MAX_ARG_LENGTH  50

char **parse_input(char *);

int handle_command(char **);

int read_in_history(FILE *);

#endif