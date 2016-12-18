#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "string.h"
#include "dictionnary.h"

dictionnary_t* new_dictionnary(int max_size) {
    dictionnary_t* d = malloc(sizeof(*d));
    assert(d);
    
    d->strings = malloc(sizeof(*d->strings) * max_size);
    assert(d->strings);
    memset(d->strings, 0, max_size * sizeof(*d->strings));
    
    d->size = 0;
    d->max_size = max_size;
    
    return d;
}

/**
 * Ajoute une entree au dictionnaire.
 */
int add_in_dictionnary(dictionnary_t* d, string_t* str) {
    string_t** tmp = NULL;
    if (d->size == d->max_size) {
        tmp = realloc(d->strings, (d->max_size * 2) * sizeof(*d->strings));
        assert(d->strings);
        
        if (tmp == NULL) {
            fprintf(stderr, "Erreur lors de la reallocation du "
            "dictionnaire.\n");
            return 0;
        }
        d->strings = tmp;
        d->max_size *= 2;
    }
    
    d->strings[d->size++] = copy_string(str);
        
    return 1;
}

/**
 * Cherche une entree dans le dictionnaire
 */
int find_in_dictionnary(dictionnary_t* d, string_t* str) {
    int i;
    
    if (str->length == 1)
        return *str->str;
    
    for (i = 0; i < d->size; i++) {
        if(d->strings[i] == NULL || d->strings[i]->str == NULL)
            return -2;
        if (compare_strings(str, d->strings[i]) == 0)
            return i + 256;
    }
    
    return -1;
}

/**
 * Libère la memoire allouee pour le dictionnaire.
 */
void free_dictionnary(dictionnary_t* d, int freeStrings) {
    int i;
    
    if (freeStrings)
        for (i = 0; i < d->size; i++)
            free_string(d->strings[i]);
    
    free(d);
}