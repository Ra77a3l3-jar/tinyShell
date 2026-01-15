#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>

// Remove trailing newline from input
void trim_newline(char *str);

void sigint_handle(int sig);

// Returns a string holded inside an enviormental variable
char *extract_variable_env(const char *token);

// Switches the $VARNAME in the input with the value got from getenv()
char *switch_name_value(char *input, char *value);

#endif
