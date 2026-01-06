#include "../include/prompt.h"
#include "../include/parser.h"
#include "../include/execute.h"
#include "../include/builtins.h"
#include <stdlib.h>

int main(void) {

    while(1) {
        char *input = NULL;

        input = read_input();        

        char **argv;
        int argc = 0;

        argv = parse_input(input, &argc);

        if(is_builtin(argv[0])) {
            run_builtin(argv);
        } else {
            execute_external(argv);
        }
                
        free_argv(argv, argc);
    }
}
