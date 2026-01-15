#include "../include/prompt.h"
#include "../include/execute.h"
#include "../include/pipe.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(void) {

    signal(SIGINT, sigint_handle);

    while(1) {
        char *input = NULL;

        input = read_input();        

        if(!input) continue;

        if(input[0] == '\0') {
            free(input);
            continue;
        }


        char *new_input = NULL;
        // TODO check if the dollars is there and check if the env value is not null if null just continue like normal

        char *env_var = extract_variable_env(input);
        if(env_var) {
            new_input = switch_name_value(input, env_var);
        } else {
            new_input = strdup(input);
        }

        if(!new_input) continue;

        int num_cmds = 0;
        char **cmds = split_pipes(new_input, &num_cmds);

        execute_external(cmds, &num_cmds);

        free(input);
        free(new_input);
    }
}
