#ifndef EXECUTE_H
#define EXECUTE_H

#include "../include/history.h"

// Execute commands and builtins
void execute_external(char **cmds, int *num_cmds, History *h);

#endif
