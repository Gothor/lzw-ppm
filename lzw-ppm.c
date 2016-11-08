#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lzw-ppm.h"

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
    $ Lecture
*******************************************************************************/

/**
 * Vérifie si le caractère envoyé est un séparateur valide (espace, tabulation,
 * retour chariot).
 */
static int is_a_separator(char c) {
    if (c == ' ' || c == '\t' || c == '\n')
        return 1;
    return 0;
}

/**
 * Saute la lecture des commentaires.
 */
static char skip_comments() {
    char c;
    while ((c = fgetc(source_file)) == '#') {
        do {
            c = fgetc(source_file);
        } while(c != '\n' && c != EOF);
        // Si on est arrivé à la fin du fichier, on le signale
        if (c == EOF)
            return c;
    }
    return c;
}

/**
 * Lit l'en-tête du fichier.
 */
static int read_header() {
    char c;
    
    c = skip_comments();
    
    // Lecture du nombre magique
    if (c != 'P') {
        fprintf(stderr, "Format de fichier incorrect : le premier caractère " \
        "doit être un 'P'.");
        return -1;
    }
    if ((c = fgetc(source_file)) != '6') {
        fprintf(stderr, "Format de fichier incorrect : le second caractère " \
        "doit être un '6'.");
        return -1;
    } else {
        encoding = BINARY;
    }
    c = fgetc(source_file);
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : le nombre magique " \
        "(P%d) doit être suivi d'un séparateur (espace, tabulation, " \
        "nouvelle ligne).", encoding);
        return -1;
    }
    
    c = skip_comments();
    ungetc(c, source_file);
    
    // Lecture des caractéristiques de l'image (taille, nombre de couleurs)
    while (isdigit(c = fgetc(source_file))) {
        image_width *= 10;
        image_width += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s la largeur de l'image.", image_width ? "après" : "avant");
        return -1;
    }
    while (isdigit(c = fgetc(source_file))) {
        image_height *= 10;
        image_height += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s la hauteur de l'image.", image_height ? "après" : "avant");
        return -1;
    }
    while (isdigit(c = fgetc(source_file))) {
        image_levels *= 10;
        image_levels += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s les niveaux de couleurs de l'image.",
        image_levels ? "après" : "avant");
        return -1;
    }
    
    fprintf(stderr, "Hauteur de l'image : %dpx\n", image_height);
    fprintf(stderr, "Largeur de l'image : %dpx\n", image_width);
    fprintf(stderr, "Niveaux de l'image : %d\n", image_levels + 1);
    
    c = skip_comments();
    ungetc(c, source_file);
    
    return 0;
}

/**
 * Effectue une compression LZW sur un fichier PPM.
 */
int lzw_ppm(char* src, char* dst) {
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
    
    if (read_header() == -1) {
        fclose(source_file);
        fclose(destination_file);
        return -1;
    }
    
    fclose(source_file);
    fclose(destination_file);
    
    return 0;
}