#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bit_writer.h"
#include "string.h"
#include "dictionnary.h"
#include "lzw.h"

#define DICTIONNARY_SIZE_INIT   256
#define NEEDED_BITS_INIT        8

#define MAX(a,b)                ((a) >= (b) ? (a) : (b))

// Static variables

static FILE*            source_file = NULL;
static FILE*            destination_file = NULL;

static int              needed_bits;

static string_t*        current_word = NULL;
static int              current_word_max_size;
static unsigned char    current_char;

static dictionnary_t*   dictionnary = NULL;

// Prototypes

static void             lzw_ppm_init();
static void             lzw_ppm_free();
static void             extend_current_word(char);

/**
 * Initialise les variables pour lzw_ppm
 */
static void lzw_ppm_init() {
    flush_buffer(NULL);
    
    current_word = (string_t*) malloc(sizeof(*current_word));
    current_word->str = (char*) malloc(DICTIONNARY_SIZE_INIT *
        sizeof(*current_word->str));
    current_word->length = 0;
    current_word_max_size = DICTIONNARY_SIZE_INIT;
    
    dictionnary = new_dictionnary(DICTIONNARY_SIZE_INIT);
    
    needed_bits = NEEDED_BITS_INIT;
}

/**
 * Libère les variables pour lzw_ppm
 */
static void lzw_ppm_free() {
    free_string(current_word);
    current_word_max_size = 0;
    
    free_dictionnary(dictionnary, 1);
    
    needed_bits = 0;
}

static void extend_current_word(char c) {
    char* tmp = NULL;
    if (current_word->length + 1 >= current_word_max_size) {
        tmp = realloc(current_word->str, current_word_max_size +
            DICTIONNARY_SIZE_INIT);
        if (tmp == NULL) {
            lzw_ppm_free();
            exit(-1);
	    }
        current_word->str = tmp;
        current_word_max_size += DICTIONNARY_SIZE_INIT;
    }
    
    *(current_word->str + current_word->length++) = c;
}

static void copy_string_to_current(string_t* str) {
    char* tmp = NULL;
    int i;
    while (str->length > current_word_max_size) {
        tmp = realloc(current_word->str, current_word_max_size + DICTIONNARY_SIZE_INIT);
        if (tmp == NULL) {
            lzw_ppm_free();
            exit(-1);
	    }
	    current_word->str = tmp;
	    current_word_max_size += DICTIONNARY_SIZE_INIT;
    }
    
    for (i = 0; i < str->length; i++)
        current_word->str[i] = str->str[i];
    current_word->length = str->length;
}

/**
 * Effectue une compression LZW sur un fichier PPM.
 */
int lzw_ppm(FILE* src, FILE* dst) {
    unsigned int pos = 0;
    int newpos;
    int i;
    int t;
    
    source_file = src;
    destination_file = dst;
    if (source_file == NULL) {
        fprintf(stderr, "Aucun fichier source transmis.\n");
        return -1;
    }
    if (destination_file == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis.\n");
        return -1;
    }

    lzw_ppm_init();
    
    // LZW
    while((t = fgetc(source_file)) != EOF) {
        current_char = (unsigned char) t;
        extend_current_word(current_char);
        #ifdef DEBUG
        fprintf(stderr, "\nMot : ");
        print_string(current_word);
        fprintf(stderr, "\n");
        #endif
        if ((newpos = find_in_dictionnary(dictionnary, current_word)) >= 0) {
            #ifdef DEBUG
            fprintf(stderr, "trouvé (%d)\n\n", newpos);
            #endif
            pos = (unsigned int) newpos;
        }
        else {
            if (newpos == -2) {
                fprintf(stderr, "Problème de dictionnaire :");
                for (i = 0; i < dictionnary->size; i++)
                    fprintf(stderr, "(%d)%s", i, dictionnary->strings[i]->str);
                lzw_ppm_free();
                return -1;
            }
            #ifdef DEBUG
            fprintf(stderr, "Pas trouvé (%d, %d bits) - Ajout du mot n°%d : ",
                pos, needed_bits, dictionnary->size);
            print_string(current_word);
            fprintf(stderr, "\n");
            #endif
            write_bits(pos, needed_bits, destination_file);
            add_in_dictionnary(dictionnary, current_word);
    
            // Si la taille du dictionnaire a atteint une puissance de 2,
            // on augmente le nombre de bits à ecrire.
            // 1 >= 1 (2); 2 >= 2 (4); 3 >= 4 (4); 4 >= 4 (8)
            if (255 + dictionnary->size >= (1 << needed_bits) - 1)
                needed_bits++;
            
            // On reinitialise le mot courant avec le nouveau caractère
            *current_word->str = current_char;
            current_word->length = 1;
            pos = current_char;
        }
    }
    #ifdef DEBUG
    fprintf(stderr, "Reste : %d\n", pos);
    #endif
    write_bits(pos, needed_bits, destination_file);
    flush_buffer(destination_file);
    
    lzw_ppm_free();
    
    return 0;
}

/**
 * Effectue une décompression LZW sur un fichier PPM.
 */
int unlzw_ppm(FILE* src, FILE* dst) {
    unsigned int code;
    int i;
    string_t* s = NULL;
    
    source_file = src;
    destination_file = dst;
    if (source_file == NULL) {
        fprintf(stderr, "Aucun fichier source transmis.\n");
        return -1;
    }
    if (destination_file == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis.\n");
        return -1;
    }
    
    lzw_ppm_init();
    
    // LZW
    current_char = fgetc(source_file);
    if (current_char == EOF) {
        lzw_ppm_free();
        fclose(source_file);
        fclose(destination_file);
    }
    fputc(current_char, destination_file);
    
    extend_current_word(current_char);
    needed_bits++;
    while((code = read_bits(source_file, needed_bits)) != -1) {
        #ifdef DEBUG
        fprintf(stderr, "Code (%d) : ", code);
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        if (code > 255 && (code - 255) <= dictionnary->size) {
            s = copy_string(dictionnary->strings[code - 256]);
        }
        else if (code > 255) {
            s = malloc(sizeof(*s));
            assert(s);
            s->str = malloc((current_word->length + 1) * sizeof(*s->str));
            assert(s->str);
            
            for (i = 0; i < current_word->length; i++)
                s->str[i] = current_word->str[i];
            
            s->str[current_word->length] = s->str[0];
            s->length = current_word->length + 1;
        }
        else {
            s = malloc(sizeof(*s));
            assert(s);
            s->str = malloc(sizeof(*s->str));
            assert(s->str);
            s->str[0] = (char) code;
            s->length = 1;
        }
        
        for (i = 0; i < s->length; i++)
            fputc(s->str[i], destination_file);
        extend_current_word(s->str[0]);
        add_in_dictionnary(dictionnary, current_word);
        if (255 + dictionnary->size >= (1 << needed_bits) - 2)
            needed_bits++;
        #ifdef DEBUG
        fprintf(stderr, "New char = %c\n", s->str[0]);
        fprintf(stderr, "Needed bits = %d\n", needed_bits);
        fprintf(stderr, "Dictionnary->size = %d :\n", dictionnary->size);
        if (dictionnary->size > 10)
            fprintf(stderr, "...\n");
        for (i = MAX(dictionnary->size - 10, 0); i < dictionnary->size; i++) {
            fprintf(stderr, "- (%d)", i + 256);
            print_string(dictionnary->strings[i]);
            fprintf(stderr, "\n");
        }
        #endif
        //free_string(current_word);
        //current_word = copy_string_to_current(s);
        copy_string_to_current(s);
        free_string(s);
    }

    lzw_ppm_free();
    
    return 0;
}
