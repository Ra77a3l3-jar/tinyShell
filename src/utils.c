#include "../include/utils.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void trim_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

volatile sig_atomic_t got_sigint = 0;

void sigint_handle(int sig) {
    (void)sig;
    got_sigint = 1;
}

// Returns a string holded inside an enviormental variable
char *extract_variable_env(const char *token) {
    if(!token) return NULL;

    const char *doll = strchr(token, '$'); // Finds first $
    if(!doll) return NULL;

    doll++; // Point pointer after $

    size_t len = strcspn(doll, " !\"\'$&|<>"); // Size till none of these char are included

    if(len == 0) return  NULL;

    char varname[256] = {0};
    if(len > 255) {
        len = 255;
    }
    strncpy(varname, doll, len); // Copy the string in doll for len chars
    varname[len] = '\0';
    
    return getenv(varname); // Return the value of the env variable with name varname
}

char *switch_name_value(char *input, char *value) {
    if(!input || !value) {
        return (char *)input;
    }

    const char *doll = strchr(input, '$');
    if(!doll) {
        return strdup(input); // No $VARNAME found
    }

    const char *end = doll + 1;
    while(*end && ((*end >= 'A' && *end <= 'Z') ||
                   (*end >= 'a' && *end <= 'z') ||
                   (*end >= '0' && *end <= '9') ||
                    *end == '_')) {
        end++;
    }

    size_t before_len = doll - input;
    size_t value_len = strlen(value);
    size_t end_len = strlen(end);

    char *result = malloc(before_len + value_len + end_len + 1);
    if(!result) return NULL;

    memcpy(result, input, before_len);
    memcpy(result + before_len, value, value_len);
    memcpy(result + before_len + value_len, end, end_len);

    return result;
}
