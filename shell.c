#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **parse_input(char *);

int main() {

    char *input = "";
    char **commands;
    pid_t child;
    int child_stat = 0;
    int exec_value = 0;

    while (1) {
        input = readline("seashell> ");
        commands = parse_input(input);

        child = fork();

        if (child == 0) {
            exec_value = execvp(commands[0], commands);
            if (exec_value == -1) {
                perror("couldn't execute command");
                exit(1);
            }
        }
        else {
            waitpid(child, &child_stat, WUNTRACED);
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