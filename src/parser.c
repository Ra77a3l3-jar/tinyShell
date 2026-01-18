#include "../include/parser.h"
#include "../include/shell.h"
#include <stdlib.h>
#include <string.h>

char **parse_input(char *input, int *argc) {
    if(!argc) return NULL;

    char **argv = malloc(sizeof(char *) * MAX_ARGS);
    if(!argv) return NULL;

    *argc = 0;
    char *tokenStatus = NULL;

    char *token = strtok_r(input, " \n", &tokenStatus);
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
        token = strtok_r(NULL, " \n", &tokenStatus);
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

char **split_semicolon(char *input, int *splits) {
    *splits = 1;
    for(int i = 0; input[i]; i++) {
        if(input[i] == ';') {
            (*splits)++;
        }
    }

    char **semi_cmds = malloc(sizeof(char *) * *splits);
    char *tokenStatus = NULL;
    char *token = strtok_r(input, ";", &tokenStatus);
    int argc = 0;

    while(token && argc < *splits) {
        char *trimmed = token;
        while(*trimmed == ' ' || *trimmed == '\t') {
            trimmed++;
        }
        char *end = trimmed + strlen(trimmed) - 1;
        while(end > trimmed && (*end == ' ' || *end == '\t' || *end == '\n')) {
            end--;
        }
        *(end + 1) = '\0';
        semi_cmds[argc++] = strdup(trimmed);
        token = strtok_r(NULL, ";", &tokenStatus);
    }
    return semi_cmds;
}
