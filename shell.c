#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    char *input = "";

    while(1) {
        input = readline("seashell> ");
        printf("%s\n", input);
    }

    return 0;
}