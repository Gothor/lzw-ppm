#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "string.h"

int compare_strings(string_t* s1, string_t* s2) {
    int i;
    string_t* s;
    if (s2->length < s1->length) {
        s = s2;
        s2 = s1;
        s1 = s;
    }
    for (i = 0; i < s1->length; i++) {
        if (s1->str[i] < s2->str[i])
            return -1;
        if (s1->str[i] > s2->str[i])
            return 1;
    }
    if (s1->length != s2->length)
        return -1;
    return 0;
}

string_t* copy_string(string_t* str) {
    int i;
    
    string_t* res = malloc(sizeof(*res));
    assert(res);
    res->length = str->length;
    res->str = malloc(res->length * sizeof(*res->str));
    assert(res->str);
    
    for (i = 0; i < str->length; i++)
        res->str[i] = str->str[i];
    
    return res;
}

#ifdef DEBUG
    void print_string(string_t* s) {
        int i;
        
        for (i = 0; i < s->length; i++)
            fputc(s->str[i], stderr);
    }
#endif

void free_string(string_t* s) {
    if (s == NULL)
        return ;
    
    if (s->str != NULL)
        free(s->str);
    
    free(s);
}