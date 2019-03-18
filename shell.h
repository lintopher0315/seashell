#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>

#define MAX_ARGS        1024
#define MAX_HISTORY     1000
#define MAX_ARG_LENGTH  50
#define MAX_USERNAME    32
#define RED             "\x1b[31m"
#define GREEN           "\x1b[32m"
#define YELLOW          "\x1b[33m"
#define BLUE            "\x1b[34m"
#define MAGENTA         "\x1b[35m"
#define CYAN            "\x1b[36m"
#define RESET           "\x1b[0m"

char **parse_input(char *);

int handle_command(char **);

int read_in_history(FILE *);

int append_history(char *);

int print_history(void);

int write_in_history(FILE *);

char *replace_tilde(char *);

#endif