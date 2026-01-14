#ifndef BUILTINS_H
#define BUILTINS_H

#define MAX_CWD 1024

// Returns 1 if cmd is a builtin, 0 otherwise
int is_builtin(const char *cmd);

// Executes builtin, returns 1 if executed
int run_builtin(char **argv);

// Changes directory
static void builtin_cd(char **argv);

// Exits the shell
static void builtin_exit(char **argv);

// Prints the current working directory
static void builtin_pwd();

// Prints arguments to stdout followed by a newline
static void builtin_echo(char **argv);

// Sets enviormetal variables
static void builtin_export(char **argv);

// Removes an enviormental variable
static void builtin_unset(char **argv);

#endif
