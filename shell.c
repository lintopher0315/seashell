#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **history;
int history_size = 0;

int main() {

    char *input = "";
    char **commands;
    int command_ret = 0;

    FILE *fp = NULL;

    history = calloc(MAX_HISTORY * sizeof(char *), 1);

    if (access("seashell_history.txt", F_OK) != -1) {
        fp = fopen("seashell_history.txt", "ab+");
        if (fp == NULL) {
            printf("couldn't access history\n");
            free(history);
            return 0;
        }
        
        if (read_in_history(fp) != 0) {
            printf("couldn't read in history\n");
            fclose(fp);
            fp = NULL;
            free(history);
            return 0;
        }
    }
    else {
        fp = fopen("seashell_history.txt", "ab+");
        if (fp == NULL) {
            printf("couldn't access history\n");
            free(history);
            return 0;
        }
    }

    fclose(fp);
    fp = NULL;

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

    free(history);

    return 0;
}

char **parse_input(char *input) {
    char **args = malloc(MAX_ARGS * sizeof(char *));
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

int read_in_history(FILE *file_pointer) {
    int scan_val = 0;
    int num_items = 0;
    int history_loc = 0;
    char cmd[MAX_ARG_LENGTH];

    if (fscanf(file_pointer, "%d\n", &num_items) == 1) {
        if (num_items > MAX_HISTORY) {
            num_items = MAX_HISTORY;
        }
        history_size = num_items;
    }
    while (((scan_val = fscanf(file_pointer, "%s\n", cmd)) != EOF) && history_loc < MAX_HISTORY) {
        if (scan_val == 1) {
            history[history_loc] = malloc(MAX_ARG_LENGTH * sizeof(char));
            strcpy(history[history_loc], cmd);
        }
        else {
            return -1;
        }
        history_loc++;
    }
    return 0;
}