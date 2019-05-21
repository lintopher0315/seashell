#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <linux/limits.h>

char **history;
int history_size = 0;
const char *home_dir;
char origin_dir[PATH_MAX];
char curr_dir[PATH_MAX];
char history_path[PATH_MAX];
char username[MAX_USERNAME];

int main() {

    char *input = "";
    char **commands;
    int command_ret = 0;

    if ((home_dir = getenv("HOME")) == NULL) {
        home_dir = getpwuid(getuid())->pw_dir;
    }

    if (getcwd(curr_dir, sizeof(curr_dir)) == NULL || getcwd(origin_dir, sizeof(origin_dir)) == NULL) {
        printf(RED "couldn't get working directory\n" RESET);
        return 0;
    }

    strcpy(history_path, origin_dir);
    strcat(history_path, "/seashell_history.txt");
    strcpy(username, getpwuid(getuid())->pw_name);

    FILE *fp = NULL;

    history = calloc(MAX_HISTORY * sizeof(char *), 1);

    if (access(history_path, F_OK) != -1) {
        fp = fopen(history_path, "ab+");
        if (fp == NULL) {
            printf(RED "couldn't access history\n RESET");
            for (int i = 0; i < history_size; i++) {
                free(history[i]);
            }
            free(history);
            return 0;
        }
        
        if (read_in_history(fp) != 0) {
            printf(RED "couldn't read in history\n" RESET);
            fclose(fp);
            fp = NULL;
            for (int i = 0; i < history_size; i++) {
                free(history[i]);
            }
            free(history);
            return 0;
        }
    }
    else {
        fp = fopen(history_path, "ab+");
        if (fp == NULL) {
            printf(RED "couldn't access history\n" RESET);
            for (int i = 0; i < history_size; i++) {
                free(history[i]);
            }
            free(history);
            return 0;
        }
    }

    fclose(fp);
    fp = NULL;

    char prompt[strlen(curr_dir) + 1];
    while (1) {
        strcpy(prompt, username);
        strcat(prompt, ":");
        strcat(prompt, curr_dir);
        strcat(prompt, "$ ");
        input = readline(prompt);

        append_history(input);
        commands = parse_input(input);

        command_ret = handle_command(commands);

        if (command_ret == -1) {
            free(input);
            int index = 0;
            while (commands[index] != NULL) {
                free(commands[index]);
                index++;
            }
            free(commands);
            break;
        }
        free(input);
        int index = 0;
        while (commands[index] != NULL) {
            free(commands[index]);
            index++;
        }
        free(commands);
    }

    fp = fopen(history_path, "w");
    if (fp == NULL) {
        printf(RED "couldn't access history\n" RESET);
        for (int i = 0; i < history_size; i++) {
            free(history[i]);
        }
        free(history);
        return 0;
    }

    if (write_in_history(fp) != 0) {
        printf(RED "couldn't write history\n" RESET);
        for (int i = 0; i < history_size; i++) {
            free(history[i]);
        }
        free(history);
        return 0;
    }

    fclose(fp);
    fp = NULL;

    for (int i = 0; i < history_size; i++) {
        free(history[i]);
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
        char *tilde_check = strchr(token, '~');

        if (tilde_check != NULL) {
            args[element] = replace_tilde(token);
        }
        else {
            args[element] = malloc(MAX_ARGS * sizeof(char));
            strcpy(args[element], token);
        }
        element++;
        token = strtok(NULL, " ");
    }
    args[element] = NULL;

    return args;
}

char *replace_tilde(char *input) {
    char *replace = malloc(MAX_ARGS * sizeof(char));
    int position = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '~') {
            strcat(replace, home_dir);
            position += strlen(home_dir);
        }
        else {
            replace[position] = input[i];
            position++;
        }
    }
    replace[position] = '\0';
    return replace;
}

int handle_command(char **input) {
    pid_t child;
    int child_stat = 0;
    int exec_value = 0;

    if (strcmp(input[0], "cd") == 0) {
        if (input[1] == NULL) {
            chdir(home_dir);
        }
        else {
            if (chdir(input[1]) == -1) {
                printf(RED "%s: not a valid directory\n" RESET, input[1]);
                return 0;
            }
        }
        if (getcwd(curr_dir, sizeof(curr_dir)) == NULL) {
            printf(RED "couldn't get working directory" RESET);
            return 0;
        }
    }
    else if (strcmp(input[0], "exit") == 0) {
        return -1;
    }
    else if (strcmp(input[0], "history") == 0) {
        print_history();
    }
    else {
        child = fork();

        if (child == 0) {
            exec_value = execvp(input[0], input);
            if (exec_value == -1) {
                perror(RED "couldn't execute command" RESET);
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
    while (((scan_val = fscanf(file_pointer, "%[^\n]\n", cmd)) != EOF) && history_loc < MAX_HISTORY) {
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

int append_history(char *command) {
    if (history_size >= MAX_HISTORY) {
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(history[i], history[i + 1]);
        }

        strcpy(history[MAX_HISTORY - 1], command);
    }
    else {
        history[history_size] = malloc(MAX_ARG_LENGTH * sizeof(char));
        strcpy(history[history_size], command);
        history_size++;
    }
    return 0;
}

int print_history(void) {
    for (int i = 0; i < history_size; i++) {
        printf("%d\t%s\n", i + 1, history[i]);
    }
    return 0;
}

int write_in_history(FILE *file_pointer) {
    int write_num = fprintf(file_pointer, "%d\n", history_size);
    if (write_num < 0) {
        return -1;
    }
    int write_history = 0;
    for (int i = 0; i < history_size; i++) {
        write_history = fprintf(file_pointer, "%s\n", history[i]);
        if (write_history < 0) {
            return -1;
        }
    }
    return 0;
}
