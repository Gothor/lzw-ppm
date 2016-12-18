#ifndef DICTIONNARY_H_INCLUDED
#define DICTIONNARY_H_INCLUDED

struct dictionnary {
    string_t**  strings;
    int         size;
    int         max_size;
};
typedef struct dictionnary dictionnary_t;

dictionnary_t*  new_dictionnary(int max_size);
int             find_in_dictionnary(dictionnary_t* d, string_t* str);
int             add_in_dictionnary(dictionnary_t* d, string_t* str);
void            free_dictionnary(dictionnary_t* d, int freeStrings);

#endif // DICTIONNARY_H_INCLUDED