#ifndef PROMPT_H
#define PROMPT_H

#define RED     "\033[0;31m"
#define BLUE    "\033[0;34m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define PURPLE "\033[0;35m"
#define RESET   "\033[0m"

// Read input line from user
// Returns a dynamically allocated string (caller must free)
char *read_input(void);

#endif
