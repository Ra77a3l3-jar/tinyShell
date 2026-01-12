#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>

// Remove trailing newline from input
void trim_newline(char *str);

void sigint_handle(int sig);

#endif
