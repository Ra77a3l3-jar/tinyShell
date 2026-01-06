#include "../include/shell.h"
#include "sys/types.h"
#include "unistd.h"
#include <stdio.h>

int main(void) {

    while(1) {
        char input[MAX_INPUT];
        printf("tShell > ");
        fflush(stdout);

        if(!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Update the first occurence of \n with a NULL terminator
        input[strcspn(input, "\n")] = '\0';

        if(strcmp(input, "exit") == 0) {
            break;
        }

        char *argv[MAX_ARGS];
        char *tokenStatus = NULL;
        int argc = 0;

        char *token = strtok_r(input, " ", &tokenStatus);
        while (token && argc < MAX_ARGS - 1) {
            argv[argc++] = token;
            token = strtok_r(NULL, " ", &tokenStatus);
        }
        argv[argc] = NULL;

        pid_t pid = fork();

        if(pid == 0) {
            execvp(argv[0], argv);
            perror("execvp");
            exit(1);
        } else if(pid > 0) {
            wait(NULL);
        } else {
            perror("fork");
        }
    }
}
