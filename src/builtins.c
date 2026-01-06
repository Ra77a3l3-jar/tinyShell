#include "../include/builtins.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int is_builtin(const char *cmd) {
    if(!cmd) return 1;

    return (
        strcmp(cmd, "cd") == 0 ||
        strcmp(cmd, "exit") == 0
    );
} 

int run_builtin(char **argv) {
    if(!argv || !argv[0]) return 1;

    if(strcmp(argv[0], "exit") == 0) {
        builtin_exit(argv);
        return 0;
    }

    if(strcmp(argv[0], "cd") == 0) {
        builtin_cd(argv);
        return 0;
    }
    return 0;
}

static void builtin_cd(char **argv) {
    char *path = NULL;

    if(!argv[1]) {
        path = getenv("HOME");
        if(!path) {
            perror("getenv");
            return;
        }
    } else {
        path = argv[1];
    }

    if(chdir(path) != 0) {
        perror("cd");
    }
}

static void builtin_exit(char **argv) {
    int status = 0;

    // Checks if the second argument is present ex: (exit 1)
    if(argv[1]) {
        status = atoi(argv[1]);
    }

    exit(status);
}
