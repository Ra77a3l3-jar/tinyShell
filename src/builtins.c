#include "../include/builtins.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int is_builtin(const char *cmd) {
    if(!cmd) return 1;

    return (
        strcmp(cmd, "cd") == 0 ||
        strcmp(cmd, "exit") == 0 ||
        strcmp(cmd, "pwd") == 0 ||
        strcmp(cmd, "echo") == 0 ||
        strcmp(cmd, "export") == 0 ||
        strcmp(cmd, "unset") == 0 ||
        strcmp(cmd, "history") == 0
    );
} 

int run_builtin(char **argv) {
    if(!argv || !argv[0]) return 1;

    if(strcmp(argv[0], "exit") == 0) {
        builtin_exit(argv);
        return 0;
    } else if(strcmp(argv[0], "cd") == 0) {
        builtin_cd(argv);
        return 0;
    } else if(strcmp(argv[0], "pwd") == 0) {
        builtin_pwd();
        return 0;
    } else if(strcmp(argv[0], "echo") == 0) {
        builtin_echo(argv);
        return 0;
    } else if(strcmp(argv[0], "export") == 0) {
        builtin_export(argv);
        return 0;
    } else if(strcmp(argv[0], "unset") == 0) {
        builtin_unset(argv);
        return 0;
    } else if(strcmp(argv[0], "history") == 0) {
        builtin_history(argv);
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

static void builtin_pwd() {

    char cwd[MAX_CWD];
    
    if(!getcwd(cwd, MAX_CWD)) {
        perror("pwd");
        return;
    }

    printf("%s\n", cwd);
}

static void builtin_echo(char **argv) {
    for (int i = 1; argv[i]; i++) {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" ");
    }
    printf("\n");
}

static void builtin_export(char **argv) {

    char *var_val = strchr(argv[1], '=');
    if(!var_val) return;

    *var_val = '\0';
    if(setenv(argv[1], var_val + 1, 1) == -1) { // The third argument in setenv allows to decide if the var can be ovewritten or not 1(yes) 0(no)
        printf(" Error: could not set the enviormental variable.\n");
    }
}

static void builtin_unset(char **argv) {
    if(!argv[1]) return;

    if(unsetenv(argv[1]) == -1) {
        printf(" Error: could not unset the enviormental variable.\n");
    }
}

static void builtin_history(History *h) {
    if(h->size == 0) return;

    for(size_t i = 0; i < h->size; i++) {
        size_t idx = (h->head - h->size + i + h->capacity) % h->capacity;
        printf("%zu  %s\n", i + 1, h->items[idx]);
    }
}
