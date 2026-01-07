#include "../include/prompt.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/git.h"
#include <stdio.h>
#include <pwd.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

char *read_input() {
    char *input = malloc(sizeof(char) * MAX_INPUT);
    if(!input) {
        perror("malloc");
        exit(1);
    }

    struct passwd *pw = getpwuid(getuid());
    const char *user = pw ? pw->pw_name : "unknown";

    char *cwd = malloc(sizeof(char) * MAX_CWD);
    if(!getcwd(cwd, MAX_CWD)) {
        snprintf(cwd, sizeof(cwd), "?");
    }

    char *last_slash = strrchr(cwd, '/');

    char *folder = (last_slash) ? last_slash + 1 : cwd;

    char *branch = get_branch();

    if(branch) {
        trim_newline(branch);
        printf(RED "@%s " GREEN "➜ " BLUE "%s " YELLOW "git(" PURPLE "%s" YELLOW ") " RESET, user, folder, branch);
    } else {
        printf(RED "@%s " GREEN "➜ " BLUE "%s : " RESET, user, folder);
    }
    free(branch);

    fflush(stdout);
    if(!fgets(input, MAX_INPUT, stdin)) {
        free(input);
        free(cwd);
        return NULL;
    }

    trim_newline(input);
    free(cwd);
    return input;
}
