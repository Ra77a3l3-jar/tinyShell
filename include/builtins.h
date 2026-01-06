#ifndef BUILTINS_H
#define BUILTINS_H

// Returns 1 if cmd is a builtin, 0 otherwise
int is_builtin(const char *cmd);

// Executes builtin, returns 1 if executed
int run_builtin(char **argv);

// Changes directory
static void builtin_cd(char **argv);

// Exits the shell
static void builtin_exit(char **argv);

#endif
