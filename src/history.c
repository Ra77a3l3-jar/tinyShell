#include "../include/history.h"

#include <stdlib.h>
#include <string.h>

void history_init(History *h, size_t capacity){
    h->capacity = capacity > 0 ? capacity : INIT_CAPACITY;
    h->items = malloc(h->capacity * sizeof(char *));
    h->size = 0;
    h->head = 0;
    h->cursor = 0;
    h->scratch = NULL;
    for(size_t i = 0; i < h->capacity; i++){
        h->items[i] = NULL;
    }
}

static void history_grow(History *h){
    size_t new_capacity = h->capacity * GROW_FACTOR;
    char **new_items = malloc(new_capacity * sizeof(char *));
    
    for(size_t i = 0; i < h->size; i++){
        new_items[i] = h->items[(h->head + i) % h->capacity];
    }
    for(size_t i = h->size; i < new_capacity; i++){
        new_items[i] = NULL;
    }
    
    free(h->items);
    h->items = new_items;
    h->capacity = new_capacity;
    h->head = 0;
}

void history_add(History *h, const char *cmd){
    if(!cmd || !*cmd) return;
    
    if(h->size > 0 && h->items[(h->head - 1 + h->capacity) % h->capacity] != NULL &&
        strcmp(h->items[(h->head - 1 + h->capacity) % h->capacity], cmd) == 0){
        return;
    }
    
    if(h->size == h->capacity){
        history_grow(h);
    }
    
    if(h->items[h->head] != NULL){
        free(h->items[h->head]);
    }
    
    h->items[h->head] = strdup(cmd);
    h->head = (h->head + 1) % h->capacity;
    
    if(h->size < h->capacity){
        h->size++;
    }
    
    history_reset_cursor(h);
}

const char *history_prev(History *h){
    if(h->size == 0) return NULL;
    
    if(h->cursor == 0){
        h->cursor = h->size;
    }
    
    if(h->cursor > 0){
        h->cursor--;
        size_t idx = (h->head - h->size + h->cursor + h->capacity) % h->capacity;
        return h->items[idx];
    }
    
    return NULL;
}

const char *history_next(History *h){
    if(h->size == 0) return NULL;
    
    if(h->cursor < h->size){
        h->cursor++;
        if(h->cursor == h->size){
            return h->scratch;
        }
        size_t idx = (h->head - h->size + h->cursor + h->capacity) % h->capacity;
        return h->items[idx];
    }
    
    return h->scratch;
}

void history_reset_cursor(History *h){
    h->cursor = h->size;
    if(h->scratch != NULL){
        free(h->scratch);
        h->scratch = NULL;
    }
}

void history_free(History *h){
    for(size_t i = 0; i < h->capacity; i++){
        if(h->items[i] != NULL){
            free(h->items[i]);
        }
    }
    free(h->items);
    if(h->scratch != NULL){
        free(h->scratch);
    }
}
