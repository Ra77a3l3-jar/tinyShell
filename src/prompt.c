#include "../include/prompt.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/git.h"
#include <stdio.h>
#include <pwd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

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
        printf(RED "@%s " GREEN "➜ " BLUE "%s " RESET, user, folder);
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

char *read_input_with_history(History *h) {
    char *input = malloc(sizeof(char *) * MAX_INPUT);
    if(!input) {
        perror("malloc");
        exit(1);
    }

    struct passwd *pw = getpwuid(getuid());
    const char *user = pw ? pw->pw_name : "unknown";
    
    char cwd[MAX_CWD];
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
        printf(RED "@%s " GREEN "➜ " BLUE "%s " RESET, user, folder);
    }
    free(branch);

    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    int pos = 0;
    int in_escape = 0;
    int escape_seq = 0;

    history_reset_cursor(h);

    while(1) {
        char c;
        if(read(STDIN_FILENO, &c, 1) != 1) {
            break;
        }

        if(c == '\033') {
            in_escape = 1;
            escape_seq = 0;
            continue;
        }

        if(in_escape) {
            if(escape_seq == 0 && c == '[') {
                escape_seq = 1;
                continue;
            } else if(escape_seq == 1) {
                if(c == 'A') {
                    if(h->cursor == h->size && pos > 0) {
                        if(h->scratch) free(h->scratch);
                        h->scratch = strndup(input, pos);
                    }

                    const char *prev = history_prev(h);
                    if(prev) {
                        while(pos > 0) {
                            write(STDOUT_FILENO, "\b \b", 3);
                            pos--;
                        }
                        strcpy(input, prev);
                        pos = strlen(input);
                        write(STDOUT_FILENO, input, pos);
                    }
                } else if(c == 'B') {
                    const char *next = history_next(h);
                    while(pos > 0) {
                        write(STDOUT_FILENO, "\b \b", 3);
                        pos--;
                    }
                    if(next) {
                        strcpy(input, next);
                        pos = strlen(input);
                        write(STDOUT_FILENO, input, pos);
                    } else {
                        input[0] = '\0';
                        pos = 0;
                    }
                }
                in_escape = 0;
                escape_seq = 0;
                continue;
            }
            in_escape = 0;
            escape_seq = 0;
            continue;
        }
        if(c == 127 || c == 'b') {
            if(pos > 0) {
                pos--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
        } else if(c == '\n') {
            input[pos] = '\0';
            write(STDOUT_FILENO, "\n", 1);
            break;
        } else if(c >= 32 && c < 127) {
            if(pos < MAX_INPUT - 1) {
                input[pos++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    return input;
}
