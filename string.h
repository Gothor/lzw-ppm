#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

struct string {
    char* str;
    int length;
    int num;
};
typedef struct string string_t;

#ifdef DEBUG
void print_string(string_t* s);
#endif

int         compare_strings(string_t* s1, string_t* s2);
string_t*   copy_string(string_t* str);
void        free_string(string_t* s);

#endif // STRING_H_INCLUDED