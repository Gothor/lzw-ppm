#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lzw-ppm.h"

#define DICTIONNARY_SIZE_INIT

// Variables
static FILE* source_file = NULL;
static FILE* destination_file = NULL;
encoding_e encoding = UNKNOWN;
static int image_height = 0;
static int image_width = 0;
static int image_levels;

static char buffer = 0;
static int size = 0;

// Méthodes
static int is_a_separator(char c);
static char skip_comments();
static int read_header();

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
    if (s2.length > s1.length) {
        s = s2;
        s2 = s1;
        s1 = s;
    }
    for (i = 0; i < s1.length; i++) {
        if (s1[i] < s2[i])
            return -1;
        if (s1[i] > s2[i])
            return 1;
    }
    if (s1.length != s2.length)
        return -1;
    return 0;
}

static string_t* dictionnary = NULL;
static int dictionnary_size = 0;
static int dictionnary_size_max = 256;

int find_in_dictionnary(string_t* str) {
    int i;
    for (i = 0; i < dictionnary_size; i++) {
        if (dictionnary[i].str == NULL)
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
            exit(-1);
        }
        dictionnary_size_max += DICTIONNARY_SIZE_INIT;
    }
    
    dictionnary[i] = copy_string(str);
    dictionnary_size++;
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
        fprintf(stderr, "Impossible d'ouvrir le fichier source.\n");
        return -1;
    }
    
    destination_file = fopen(dst, "w");
    if (destination_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier destination.\n");
        return -1;
    }
    
    
    
    fclose(source_file);
    fclose(destination_file);
    
    return 0;
}