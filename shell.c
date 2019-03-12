#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    char *input = "";
    char **commands;
    int command_ret = 0;

    while (1) {
        input = readline("seashell> ");
        commands = parse_input(input);

        command_ret = handle_command(commands);

        if (command_ret == -1) {
            free(input);
            free(commands);
            break;
        }
        
        free(input);
        free(commands);
    }

    return 0;
}

char **parse_input(char *input) {
    char **args = malloc(16 * sizeof(char *));
    if (args == NULL) {
        perror("malloc call unsuccessful");
        exit(1);
    }
    char *token;
    int element = 0;

    token = strtok(input, " ");

    while (token != NULL) {
        args[element] = token;
        element++;
        token = strtok(NULL, " ");
    }
    args[element] = NULL;

    return args;
}

int handle_command(char **input) {
    pid_t child;
    int child_stat = 0;
    int exec_value = 0;

    if (strcmp(input[0], "cd") == 0) {
        if (input[1] == NULL) {
            chdir(getenv("HOME"));
        }
        else {
            if (chdir(input[1]) == -1) {
                printf("%s: not a valid directory\n", input[1]);
                return 0;
            }
        }
    }
    else if (strcmp(input[0], "exit") == 0) {
        return -1;
    }
    else {
        child = fork();

        if (child == 0) {
            exec_value = execvp(input[0], input);
            if (exec_value == -1) {
                perror("couldn't execute command");
                return 0;
            }
        }
        else {
            waitpid(child, &child_stat, WUNTRACED);
        }
    }
    return 0;
}