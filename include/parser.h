#ifndef PARSER_H
#define PARSER_H

// Parse input string into argv array
// Returns NULL-terminated array of strings
char **parse_input(char *input, int *argc);

// Free argv produced by parse_input
void free_argv(char **argv, int argc);

#endif
