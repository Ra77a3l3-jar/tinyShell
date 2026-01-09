#include "../include/pipe.h"
#include <string.h>
#include <stdlib.h>

char **split_pipes(char *input, int *num_cmds) {
    *num_cmds = 1;
    for(int i = 0; input[i]; i++) {
        if(input[i] =='|') {
            (*num_cmds)++;
        }
    }

    char **cmds = malloc(sizeof(char *) * *num_cmds);
    char *tokenStatus = NULL;
    char *token = strtok_r(input, "|", &tokenStatus);
    int argc = 0;

    while(token && argc < *num_cmds) {
        char *trimmed = token;
        while(*trimmed == ' ' || *trimmed == '\t') {
            trimmed++;
        }
        char *end = trimmed + strlen(trimmed) - 1;
        while(end > trimmed && (*end == ' ' || *end == '\t' || *end == '\n')) {
            end--;
        }
        *(end + 1) = '\0';

        cmds[argc++] = strdup(trimmed);
        token = strtok_r(NULL, "|", &tokenStatus);
    }
    return cmds;
}
