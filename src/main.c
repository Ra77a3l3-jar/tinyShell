#include "../include/prompt.h"
#include "../include/execute.h"
#include "../include/pipe.h"
#include "../include/utils.h"
#include "../include/parser.h"
#include "../include/history.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(void) {

    signal(SIGINT, sigint_handle);

    History *history = history_init(0);

    while(1) {
        char *input = NULL;

        input = read_input_with_history(history);

        if(!input) continue;

        if(input[0] == '\0') {
            free(input);
            continue;
        }

        history_add(history, input);

        char *new_input = NULL;

        char *env_var = extract_variable_env(input);
        if(env_var) {
            new_input = switch_name_value(input, env_var);
        } else {
            new_input = strdup(input);
        }

        if(!new_input) continue;

        int num_splits = 0;
        char **semi_cmds = split_semicolon(new_input, &num_splits);
        
        for(int i = 0; i < num_splits; i++) {
            int num_cmds = 0;
            char **cmds = split_pipes(semi_cmds[i], &num_cmds);
            
            execute_external(cmds, &num_cmds, history);
        }

        for(int i = 0; i < num_splits; i++) {
            free(semi_cmds[i]);
        }
        free(semi_cmds);
        free(input);
        free(new_input);
    }
    history_free(history);
}
