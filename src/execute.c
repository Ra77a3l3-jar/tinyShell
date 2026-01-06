#include "../include/execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_external(char **argv) {
    if(!argv) return;

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
 
