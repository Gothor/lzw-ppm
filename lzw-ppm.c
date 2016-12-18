/**
 * @TODO React properly if realloc of dictionnary and current_word doesn't work
 * @TODO Enlever PHONY program dans le Makefile
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "lzw-ppm.h"

#define DICTIONNARY_SIZE_INIT   256
#define NEEDED_BITS_INIT        8

#define MAX(a,b)                ((a) >= (b) ? (a) : (b))

// Static variables

static FILE*            source_file = NULL;
static FILE*            destination_file = NULL;

static char             buffer = 0;
static int              size = 0;
static int              needed_bits;

static string_t*        current_word = NULL;
static int              current_word_max_size;
static unsigned char    current_char;

static string_t**       dictionnary = NULL;
static int              dictionnary_size;
static int              dictionnary_size_max;

// Prototypes

#ifdef DEBUG
static void             print_buffer();
#endif

static int              write_bits(unsigned int v, int n, FILE* f);
static void             flush_buffer(FILE* f);

static int              compare_strings(string_t* s1, string_t* s2);
static string_t*        copy_string(string_t* str);
static void             free_string(string_t* s);

static int              find_in_dictionnary(string_t* str);
static int              add_in_dictionnary(string_t* str);
static void             free_dictionnary();

static void             lzw_ppm_init();
static void             lzw_ppm_free();
static void             extend_current_word(char);

/*******************************************************************************
    $ Ecriture
*******************************************************************************/

#ifdef DEBUG
    /**
     * Affiche le contenu du buffer.
     */
    static void print_buffer() {
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
 * Ecrit les n bits de poids faible de la chaîne envoyee.
 */
static int write_bits(unsigned int v, int n, FILE* f) {
    if (n > 32)
        return -1;
    
    // Si le buffer n'est pas vide, on finit de le remplir
    if (size + n >= 8) {
        buffer |= (char) (v >> (n - (8 - size)));
        fputc(buffer, f);
        #ifdef DEBUG
        fprintf(stderr, "---- ");
        print_buffer();
        #endif
        n -= (8 - size);
        size = 0;
        buffer = 0;
    }
    // Si ce qu'on doit ecrire ne remplit pas le buffer
    else {
        buffer |= v << (8 - (n + size));
        size += n;
        return 0;
    }
    
    // On ecrit les octets qui remplissent le buffer
    while (n >= 8) {
		buffer = (char) (v >> (n % 8 + 8 * (n / 8 - 1)));
        fputc(buffer, f);
        #ifdef DEBUG
        fprintf(stderr, "-------- ");
        print_buffer();
        #endif
        n -= 8;
    }
    
    if (n > 0) {
        buffer = (char) v << (8 - n);
        size = n;
    }
    else {
        buffer = 0;
    }
    
    #ifdef DEBUG
    fprintf(stderr, "-------- Reste : ");
    print_buffer();
    #endif
    
    return 0;
}

/**
 * Vide le buffer.
 */
static void flush_buffer(FILE* f) {
    if (size != 0) {
        fputc(buffer, f);
        #ifdef DEBUG
        fprintf(stderr, "Vidage du buffer : ");
        print_buffer();
        #endif
        buffer = 0;
        size = 0;
    }
}

/*******************************************************************************
    $ Chaînes de caractères
*******************************************************************************/

static int compare_strings(string_t* s1, string_t* s2) {
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

static string_t* copy_string(string_t* str) {
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
    static void print_string(string_t* s) {
        int i;
        
        for (i = 0; i < s->length; i++)
            fputc(s->str[i], stderr);
    }
#endif

static void free_string(string_t* s) {
    if (s == NULL)
        return ;
    
    if (s->str != NULL)
        free(s->str);
    
    free(s);
}

/*******************************************************************************
    $ Dictionnaire
*******************************************************************************/

/**
 * Cherche une entree dans le dictionnaire
 */
static int find_in_dictionnary(string_t* str) {
    int i;
    
    if (str->length == 1)
        return *str->str;
    for (i = 0; i < dictionnary_size; i++) {
        if(dictionnary[i] == NULL || dictionnary[i]->str == NULL)
            return -2;
        if (compare_strings(str, dictionnary[i]) == 0)
            return i + 256;
    }
    return -1;
}

/**
 * Ajoute une entree au dictionnaire.
 */
static int add_in_dictionnary(string_t* str) {
    //static string_t** tmp = NULL;
    if (dictionnary_size == dictionnary_size_max) {
        dictionnary = realloc(dictionnary,
            (dictionnary_size_max + DICTIONNARY_SIZE_INIT) *
            sizeof(*dictionnary));
        assert(dictionnary);
        if (dictionnary == NULL) {
            fprintf(stderr, "Erreur lors de la reallocation du "
            "dictionnaire.\n");
            return 0;
        }
        dictionnary_size_max += DICTIONNARY_SIZE_INIT;
    }
    
    dictionnary[dictionnary_size++] = copy_string(str);
        
    return 1;
}

/**
 * Libère la memoire allouee pour le dictionnaire.
 */
static void free_dictionnary() {
    int i;
    
    for (i = 0; i < dictionnary_size; i++)
        free_string(dictionnary[i]);
    
    free(dictionnary);
    dictionnary_size = 0;
    dictionnary_size_max = 0;
}

/*******************************************************************************
    $ LZW
*******************************************************************************/

/**
 * Initialise les variables pour lzw_ppm
 */
static void lzw_ppm_init() {
    buffer = 0;
    size = 0;
    
    current_word = (string_t*) malloc(sizeof(*current_word));
    current_word->str = (char*) malloc(DICTIONNARY_SIZE_INIT *
        sizeof(*current_word->str));
    current_word->length = 0;
    current_word_max_size = DICTIONNARY_SIZE_INIT;
    
    dictionnary = (string_t**) malloc(sizeof(*dictionnary) *
        DICTIONNARY_SIZE_INIT);
    memset(dictionnary, 0, DICTIONNARY_SIZE_INIT * sizeof(*dictionnary));
    dictionnary_size = 0;
    dictionnary_size_max = DICTIONNARY_SIZE_INIT;
    
    needed_bits = NEEDED_BITS_INIT;
}

/**
 * Libère les variables pour lzw_ppm
 */
static void lzw_ppm_free() {
    free_string(current_word);
    current_word_max_size = 0;
    
    free_dictionnary(dictionnary);
    dictionnary_size = 0;
    dictionnary_size_max = 0;
    
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
        if ((newpos = find_in_dictionnary(current_word)) >= 0) {
            #ifdef DEBUG
            fprintf(stderr, "trouvé (%d)\n\n", newpos);
            #endif
            pos = (unsigned int) newpos;
        }
        else {
            if (newpos == -2) {
                fprintf(stderr, "Problème de dictionnaire :");
                for (i = 0; i < dictionnary_size; i++)
                    fprintf(stderr, "(%d)%s", i, dictionnary[i]->str);
                lzw_ppm_free();
                return -1;
            }
            #ifdef DEBUG
            fprintf(stderr, "Pas trouvé (%d, %d bits) - Ajout du mot n°%d : ",
                pos, needed_bits, dictionnary_size);
            print_string(current_word);
            fprintf(stderr, "\n");
            #endif
            write_bits(pos, needed_bits, destination_file);
            add_in_dictionnary(current_word);
    
            // Si la taille du dictionnaire a atteint une puissance de 2,
            // on augmente le nombre de bits à ecrire.
            // 1 >= 1 (2); 2 >= 2 (4); 3 >= 4 (4); 4 >= 4 (8)
            if (255 + dictionnary_size >= (1 << needed_bits) - 1)
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

int lus = 0;
int read_bits(FILE* f, int n) {
    int code;
    int c;
    unsigned char b;
    #ifdef ADEBUG
    int i;
    #endif
    
    if (needed_bits > 32) {
        printf("Mais vous êtes fou !\n");
        return -1;
    }
    
    if (n < size) {
        code = buffer >> (8 - n);
        buffer <<= n;
        size -= n;
        return code;
    }
    
    code = (unsigned char) buffer >> (8 - size);
    #ifdef ADEBUG
    fprintf(stderr, "-- Code :       ");
    for (i = 31; i >= 0; i--)
        fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
    fprintf(stderr, "\n");
    #endif
    n -= size;
    while (n > 8) {
        c = fgetc(f);
        buffer = (char) c;
        #ifdef DEBUG
        fprintf(stderr, "%d char lus\n", ++lus);
        #endif
        if (c == EOF) {
            return -1;
        }
        code = (code << 8);
        #ifdef ADEBUG
        fprintf(stderr, "---- Code (1) : ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        code += (unsigned char) buffer;
        #ifdef ADEBUG
        fprintf(stderr, "---- Code (2) : ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        size = 8;
        n -= 8;
    }
    
    if (n > 0) {
        c = fgetc(f);
        buffer = (char) c;
        #ifdef DEBUG
        fprintf(stderr, "%d char lus\n", ++lus);
        #endif
        if (c == EOF) {
            return -1;
        }
        code = (code << n);
        #ifdef ADEBUG
        fprintf(stderr, "------ Code :   ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        b = 0;
        b |= buffer;
        b >>= 8 - n;
        code |= b;
        #ifdef ADEBUG
        fprintf(stderr, "------ Buffer >> (8 - %d) : ", n);
        for (i = 7; i >= 0; i--)
            fprintf(stderr, "%c", (buffer >> (8 - n)) & (1 << i) ? '1' : '0');
        fprintf(stderr, " (%d)\n", size);
        fprintf(stderr, "------ Code :   ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        buffer <<= n;
        size = 8 - n;
    }
    
    #ifdef ADEBUG
    printf("------ Buffer : ");
    for (i = 7; i >= 0; i--)
        printf("%c", buffer & (1 << i) ? '1' : '0');
    printf(" (%d)\n", size);
    #endif
    
    return code;
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
        if (code > 255 && (code - 255) <= dictionnary_size) {
            s = copy_string(dictionnary[code - 256]);
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
        add_in_dictionnary(current_word);
        if (255 + dictionnary_size >= (1 << needed_bits) - 2)
            needed_bits++;
        #ifdef DEBUG
        fprintf(stderr, "New char = %c\n", s->str[0]);
        fprintf(stderr, "Needed bits = %d\n", needed_bits);
        fprintf(stderr, "Dictionnary_size = %d :\n", dictionnary_size);
        if (dictionnary_size > 10)
            fprintf(stderr, "...\n");
        for (i = MAX(dictionnary_size - 10, 0); i < dictionnary_size; i++) {
            fprintf(stderr, "- (%d)", i + 256);
            print_string(dictionnary[i]);
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
