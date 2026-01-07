#include "../include/git.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *get_branch() {
    FILE *fp;
    char *branch = malloc(sizeof(char) * MAX_BRANCH);
    if(!branch) return NULL;

    fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
    if(!fp) {
        free(branch);
        return NULL;
    }

    if(fgets(branch, MAX_BRANCH, fp)) {
        branch[strcspn(branch, "\n")] = 0;
        if(strlen(branch) == 0) {
            free(branch);
            pclose(fp);
            return NULL;
        }
    } else {
        free(branch);
        pclose(fp);
        return NULL;
    }

    pclose(fp);
    return branch;
}
