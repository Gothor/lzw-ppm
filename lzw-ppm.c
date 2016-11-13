#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "lzw-ppm.h"

#define DICTIONNARY_SIZE_INIT 256

// Static variables

static FILE* source_file = NULL;
static FILE* destination_file = NULL;

static char buffer = 0;
static int size = 0;

// Prototypes

#ifdef DEBUG
    static void  print_buffer();
#endif

static int       write_bits(unsigned int v, int n);
static void      flush_buffer();

static int       compare_strings(string_t* s1, string_t* s2);
static string_t* copy_string(string_t* str);
static void      free_string(string_t* s);

static int       find_in_dictionnary(string_t* str);
static int       add_in_dictionnary(string_t* str);
static void      free_dictionnary();

static void      lzw_ppm_init();

/*******************************************************************************
    $ Ecriture
*******************************************************************************/

#ifdef DEBUG
    /**
     * Affiche le contenu du buffer.
     */
    void print_buffer() {
        int i;
        
        fprintf(stderr, "buffer: (");
        
        for (i = 0; i < 8; i++) {
            if (size == i)
                fprintf(stderr, "[");
            fprintf(stderr, "%c", buffer & (1 << (7 - i)) ? '1' : '0');
            if (i == 3)
                fprintf(stderr, "'");
        }
        fprintf(stderr, "])\n");
    }
#endif

/**
 * Ecrit les n bits de poids faible de la chaîne envoyée.
 */
int write_bits(unsigned int v, int n) {
    if (n > 32)
        return -1;
    
    // Si le buffer n'est pas vide, on finit de le remplir
    if (size + n > 8) {
        buffer |= (char) (v >> (n - (8 - size)));
        //fputc(buffer, f);
        n -= (8 - size);
        size = 0;
    }
    // Si ce qu'on doit écrire ne remplit pas le buffer
    else {
        buffer |= v << (8 - (n + size));
        size += n;
        return 0;
    }
    
    // On écrit les octets qui remplissent le buffer
    while (n >= 8) {
        buffer = (char) (v >> (n % 8 + 8 * (n / 8 - 1)));
        //fputc(buffer, f);
        n -= 8;
    }
    
    if (n > 0) {
        buffer = v << (8 - n);
        size = n;
    }
    
    return 0;
}

/**
 * Vide le buffer.
 */

void flush_buffer() {
    buffer = 0;
    size = 0;
}

/*******************************************************************************
    $ Dictionnaire
*******************************************************************************/

struct string {
    char* str;
    int length;
};
typedef struct string string_t;

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

void free_string(string_t* s) {
    if (s == NULL)
        return ;
    
    if (s->str != NULL)
        free(s->str);
    
    free(s);
}

static string_t** dictionnary = NULL;
static int dictionnary_size = 0;
static int dictionnary_size_max = 256;

int find_in_dictionnary(string_t* str) {
    int i;
    for (i = 0; i < dictionnary_size; i++) {
        if (dictionnary[i]->str == NULL)
            return -1;
        if (compare_strings(str, dictionnary[i]) == 0)
            return i;
    }
    return -1;
}

int add_in_dictionnary(string_t* str) {
    if (dictionnary_size == dictionnary_size_max) {
        dictionnary = realloc(dictionnary, dictionnary_size_max + DICTIONNARY_SIZE_INIT);
        if (dictionnary == NULL) {
            fprintf(stderr, "Erreur lors de la reallocation du dictionnaire.\n");
            return 0;
        }
        dictionnary_size_max += DICTIONNARY_SIZE_INIT;
    }
    
    dictionnary[dictionnary_size++] = copy_string(str);
    
    return 1;
}

void free_dictionnary() {
    int i;
    
    for (i = 0; i < dictionnary_size; i++)
        free_string(dictionnary[i]);
    
    free(dictionnary);
    dictionnary_size = 0;
    dictionnary_size_max = 0;
}

/**
 * Initialise les variables pour lzw_ppm
 */

void lzw_ppm_init() {
    dictionnary = malloc(sizeof(*dictionnary) * DICTIONNARY_SIZE_INIT);
    dictionnary_size = 0;
    dictionnary_size_max = DICTIONNARY_SIZE_INIT;
}

/**
 * Effectue une compression LZW sur un fichier PPM.
 */
int lzw_ppm(char* src, char* dst) {
    lzw_ppm_init();
    
    source_file = fopen(src, "r");
    if (source_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier source (%s).\n", src);
        return -1;
    }
    
    destination_file = fopen(dst ? dst : "output.xppm", "w");
    if (destination_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier destination (%s).\n", dst);
        return -1;
    }
    
    fclose(source_file);
    fclose(destination_file);
    
    return 0;
}