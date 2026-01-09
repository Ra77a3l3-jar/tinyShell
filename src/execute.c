#include "../include/execute.h"
#include "../include/parser.h"
#include "../include/builtins.h"
#include "sys/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_external(char **cmds, int *num_cmds) {

    if(*num_cmds == 1) {
        int argc = 0;
        char **argv = parse_input(cmds[0], &argc);
        if(!argv[0]) {
            free(argv);
            free(cmds);
        }

        if(is_builtin(argv[0])) {
            run_builtin(argv);
            free(argv);
            free(cmds);
            return;
        }

        pid_t pid = fork();

        if(pid == 0) {
            execvp(argv[0], argv);
            perror("execvp");
            exit(1);
        }
        if(pid > 0) {
            wait(NULL);
        } else {
            perror("fork");
            exit(1);
        }
    } else {
        int num_pipes = *num_cmds - 1;
        int pipes[num_pipes][2];

        // Creating all the pipes
        for(int i = 0; i < num_pipes; i++) {
            if(pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        // Fork child for each command
        for(int i = 0; i < *num_cmds; i++) {
            int argc = 0;

            char **argv = parse_input(cmds[i], &argc);

            if(!argv || !argv[0]) {
                free(argv);
                continue;
            }

            // Builtin check
            if(is_builtin(argv[0])) {
                if(strcmp(argv[0], "cd") == 0 || strcmp(argv[0], "exit") == 0) {
                    fprintf(stderr, "Error: '%s' cannot be used in a pipe\n", argv[0]);
                    free(argv);
                    for(int j = 0; j < num_pipes; j++) {
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                    free(cmds);
                    return;
                }
            }

            pid_t pid = fork();
            if(pid == -1) {
                perror("fork");
                exit(1);
            }

            if(pid == 0) { // Child process for comand i
                // If not first command read from previus pipe
                if(i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                    close(pipes[i - 1][0]);
                    close(pipes[i - 1][1]);
                }

                // If not last command write to the current pipe
                if(i < *num_cmds - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                    close(pipes[i][0]);
                    close(pipes[i][1]);
                }

                // Close all unused pipes
                for(int j = 0; j < num_pipes; j++) {
                    if((i == 0 || j != i) && (i == *num_cmds - 1 || j != i)) {
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                }

                if(is_builtin(argv[0])) {
                    run_builtin(argv);
                    exit(0);
                }

                execvp(argv[0], argv);
                perror("execvp");
                exit(1);
            }
        }

        // Parent closes all the pipes and waits
        for(int i = 0; i < num_pipes; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for(int i = 0; i < *num_cmds; i++) {
            wait(NULL);
        }
        
        free(cmds);
    }
}
 
