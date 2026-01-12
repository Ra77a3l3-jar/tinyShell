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
        if(!argv || !argv[0]) {
            for(int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            for(int i = 0; i < *num_cmds; i++) {
                free(cmds[i]);
            }
            free(argv);
            free(cmds);
            return;
        }

        if(is_builtin(argv[0])) {
            run_builtin(argv);
            for(int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            for(int i = 0; i < *num_cmds; i++) {
                free(cmds[i]);
            }
            free(argv);
            free(cmds);
            return;
        }

        pid_t pid = fork();

        if(pid == 0) {
            signal(SIGINT, SIG_DFL);
            
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
        for(int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        for(int i = 0; i < *num_cmds; i++) {
            free(cmds[i]);
        }
        free(argv);
        free(cmds);
    } else {
        int num_pipes = *num_cmds - 1;
        int pipes[num_pipes][2];

        //Creating all the pipes
        for(int i = 0; i < num_pipes; i++) {
            if(pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        // Fork child for each comand
        for(int i = 0; i < *num_cmds; i++) {
            int argc = 0;
            char **argv = parse_input(cmds[i], &argc);

            if(!argv || !argv[0]) {
                for(int j = 0; j < argc; j++) {
                    free(argv[j]);
                }
                free(argv);
                continue;
            }

            // Builtin check
            if(is_builtin(argv[0])) {
                if(strcmp(argv[0], "cd") == 0 || strcmp(argv[0], "exit") == 0) {
                    fprintf(stderr, "Error: '%s' cannot be used in a pipe\n", argv[0]);
                    for(int j = 0; j < argc; j++) {
                        free(argv[j]);
                    }
                    free(argv);
                    for(int j = 0; j < num_pipes; j++) {
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                    for(int j = 0; j < *num_cmds; j++) {
                        free(cmds[j]);
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

            // Child process for command i
            if(pid == 0) {

                // If not first comand read from last pipe
                if(i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }

                // If not last command write to current pipe
                if(i < *num_cmds - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                // Close all pipes
                for(int j = 0; j < num_pipes; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                if(is_builtin(argv[0])) {
                    run_builtin(argv);
                    exit(0);
                }

                signal(SIGINT, SIG_DFL);

                execvp(argv[0], argv);
                perror("execvp");
                exit(1);
            }
            for(int j = 0; j < argc; j++) {
                free(argv[j]);
            }
            free(argv);
        }

        // Parent closes all pipes and waits
        for(int i = 0; i < num_pipes; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for(int i = 0; i < *num_cmds; i++) {
            wait(NULL);
        }

        for(int i = 0; i < *num_cmds; i++) {
            free(cmds[i]);
        }
        free(cmds);
    }
}
 
