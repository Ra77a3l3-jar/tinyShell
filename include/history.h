#ifndef HISTORY_H
#define HISTORY_H

#include <stddef.h>

#define INIT_CAPACITY 32
#define GROW_FACTOR 2

typedef struct {
    char **items;        
    size_t capacity;     
    size_t size;
    size_t head;       
    size_t cursor; 
    char *scratch;
} History;

void history_init(History *h, size_t capacity);
void history_add(History *h, const char *cmd);

const char *history_prev(History *h);
const char *history_next(History *h);

void history_reset_cursor(History *h);
void history_free(History *h);

#endif
