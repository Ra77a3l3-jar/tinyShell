#include "../include/utils.h"
#include <string.h>

void trim_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}
