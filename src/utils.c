#include "../include/utils.h"
#include <signal.h>
#include <string.h>

void trim_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

volatile sig_atomic_t got_sigint = 0;

void sigint_handle(int sig) {
    (void)sig;
    got_sigint = 1;
}
