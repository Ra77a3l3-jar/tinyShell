#include "../include/parser.h"
#include "../include/shell.h"
#include <stdlib.h>
#include <string.h>

char **parse_input(char *input, int *argc) {
    char **argv = malloc(sizeof(char *) * MAX_ARGS);
    if(!argv) return NULL;

    *argc = 0;
    char *tokenStatus = NULL;

    char *token = strtok_r(input, " ", &tokenStatus);
    while(token && *argc < MAX_ARGS - 1) {
        argv[*argc] = strdup(token);
        if(!argv[*argc]) {
            for(int i = 0; i < *argc; i++) {
                free(argv[i]);
            }
            free(argv);
            return NULL;
        }
        
        (*argc)++;
        token = strtok_r(NULL, " ", &tokenStatus);
    }
    
    argv[*argc] = NULL;
    return argv;
}

void free_argv(char **argv, int argc) {
    for(int i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
}
