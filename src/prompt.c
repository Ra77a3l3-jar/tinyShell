#include "../include/prompt.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include <stdio.h>

char *read_input() {
    char *input = malloc(sizeof(char) * MAX_INPUT);
    if(!input) {
        perror("malloc");
        exit(1);
    }

    printf(BLUE "tShell" GREEN " > " RESET);
    fflush(stdout);
    if(!fgets(input, MAX_INPUT, stdin)) {
        free(input);
        return NULL;
    }

    trim_newline(input);
    return input;
}
