#include <stdio.h>
#include <stdlib.h>
#include "ppm-exploder.h"

// Static variables

static FILE*    source_file = NULL;
static FILE*    destination_file_header = NULL;
static FILE*    destination_file_red = NULL;
static FILE*    destination_file_green = NULL;
static FILE*    destination_file_blue = NULL;

static int      image_height = 0;
static int      image_width = 0;
static int      image_levels = 0;

// Prototypes

static int      is_a_separator(char c);
static char     skip_comments();
static int      extract_header();

/*******************************************************************************
    $ Lecture
*******************************************************************************/

static int copy_char(FILE* src, FILE* dst) {
    int c = fgetc(src);
    if (c != -1)
        fputc((char) c, dst);
    return c;
}

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
    while ((c = copy_char(source_file, destination_file_header)) == '#') {
        do {
            c = copy_char(source_file, destination_file_header);
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
static int extract_header() {
    char c;
    
    c = skip_comments();
    
    // Lecture du nombre magique
    if (c != 'P') {
        fprintf(stderr, "Format de fichier incorrect : le premier caractère " \
        "doit être un 'P'.");
        return -1;
    }
    if ((c = copy_char(source_file, destination_file_header)) != '6') {
        fprintf(stderr, "Format de fichier incorrect : le second caractère " \
        "doit être un '6'.");
        return -1;
    } else {
        encoding = BINARY;
    }
    c = copy_char(source_file, destination_file_header);
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : le nombre magique " \
        "(P%d) doit être suivi d'un séparateur (espace, tabulation, " \
        "nouvelle ligne).", encoding);
        return -1;
    }
    
    c = skip_comments();
    ungetc(c, source_file);
    
    // Lecture des caractéristiques de l'image (taille, nombre de couleurs)
    while (isdigit(c = copy_char(source_file, destination_file_header))) {
        image_width *= 10;
        image_width += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s la largeur de l'image.", image_width ? "après" : "avant");
        return -1;
    }
    while (isdigit(c = copy_char(source_file, destination_file_header))) {
        image_height *= 10;
        image_height += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s la hauteur de l'image.", image_height ? "après" : "avant");
        return -1;
    }
    while (isdigit(c = copy_char(source_file, destination_file_header))) {
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

int explode_ppm(FILE* src, FILE* dst_h, FILE* dst_r, FILE* dst_g, FILE* dst_b) {
    source_file = src;
    destination_file_header = dst_h;
    destination_file_red = dst_r;
    destination_file_green = dst_g;
    destination_file_blue = dst_b;
    
    if (source_file == NULL) {
        fprintf(stderr, "Aucun fichier source transmis.\n");
        return -1;
    }
    if (destination_file_header == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour le header.\n");
        return -1;
    }
    
    extract_header();
    /*if (destination_file_red == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour la couleur "
            "rouge.\n");
        return -1;
    }
    if (destination_file_green == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour la couleur "
            "verte.\n");
        return -1;
    }
    if (destination_file_blue == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour la couleur "
            "bleue.\n");
        return -1;
    }*/
}

