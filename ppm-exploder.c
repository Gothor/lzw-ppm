#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ppm-exploder.h"
#include "color.h"

// Static variables

static FILE*    image = NULL;
static FILE*    header = NULL;
static FILE*    red = NULL;
static FILE*    green = NULL;
static FILE*    blue = NULL;

encoding_e      encoding = UNKNOWN;
static int      image_height = 0;
static int      image_width = 0;
static int      image_levels = 0;

// Prototypes

static int      is_a_separator(char c);
static int      skip_comments();
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
static int skip_comments() {
    int c;
    while ((c = fgetc(image)) == '#') {
        fputc(c, header);
        do {
            c = copy_char(image, header);
        } while(c != '\n' && c != EOF);
        // Si on est arrivé à la fin du fichier, on le signale
        printf("J'ai lu un retour a la ligne.\n");
        if (c == EOF)
            return c;
    }
    ungetc(c, image);
    return c;
}

/**
 * Lit l'en-tête du fichier.
 */
static int extract_header() {
    int c;
    
    image_height = 0;
    image_width = 0;
    image_levels = 0;
    
    c = skip_comments();
    
    // Lecture du nombre magique
    if ((c = copy_char(image, header)) != 'P') {
        fprintf(stderr, "Format de fichier incorrect : le premier caractère " \
        "doit être un 'P'. (Premier char : '%d')\n", c);
        return 1;
    }
    if ((c = copy_char(image, header)) != '6') {
        fprintf(stderr, "Format de fichier incorrect : le second caractère " \
        "doit être un '6'.");
        return 1;
    } else {
        encoding = BINARY;
    }
    c = copy_char(image, header);
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : le nombre magique " \
        "(P%d) doit être suivi d'un séparateur (espace, tabulation, " \
        "nouvelle ligne).", encoding);
        return 1;
    }
    
    c = skip_comments();
    
    // Lecture des caractéristiques de l'image (taille, nombre de couleurs)
    while (isdigit(c = copy_char(image, header))) {
        image_width *= 10;
        image_width += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s la largeur de l'image.", image_width ? "après" : "avant");
        return 1;
    }
    while (isdigit(c = copy_char(image, header))) {
        image_height *= 10;
        image_height += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s la hauteur de l'image.", image_height ? "après" : "avant");
        return 1;
    }
    while (isdigit(c = copy_char(image, header))) {
        image_levels *= 10;
        image_levels += c - '0';
    }
    if (!is_a_separator(c)) {
        fprintf(stderr, "Format de fichier incorrect : caractère non désiré " \
        "%s les niveaux de couleurs de l'image.",
        image_levels ? "après" : "avant");
        return 1;
    }
    
    #if DEBUG
    fprintf(stderr, "Hauteur de l'image : %dpx\n", image_height);
    fprintf(stderr, "Largeur de l'image : %dpx\n", image_width);
    fprintf(stderr, "Niveaux de l'image : %d\n", image_levels + 1);
    #endif
    
    c = skip_comments();
    
    return 0;
}

static int extract_content(int toHSV) {
    int i, c, color;
    
    for (i = 0; i < image_height * image_width; i++) {
        c = fgetc(image);
        if (c == EOF) {
            fprintf(stderr, "La taille de l'image ne correspond pas à celle "
                "indiquée dans le fichier.");
            return 1;
        }
        color = c << 16;
        
        c = fgetc(image);
        if (c == EOF) {
            fprintf(stderr, "La taille de l'image ne correspond pas à celle "
                "indiquée dans le fichier.");
            return 1;
        }
        color |= c << 8;
        
        c = fgetc(image);
        if (c == EOF) {
            fprintf(stderr, "La taille de l'image ne correspond pas à celle "
                "indiquée dans le fichier.");
            return 1;
        }
        color |= c;
        
        if (toHSV)
            color = color_rgb_to_hsv(color);
            
        fputc(color >> 16 & 0xFF, red);
        fputc(color >> 8 & 0xFF,  green);
        fputc(color & 0xFF,       blue);
    }
    
    return 0;
}

int explode_ppm(FILE* src, FILE* dst_h, FILE* dst_r, FILE* dst_g, FILE* dst_b,
    int toHSV) {
    image = src;
    header = dst_h;
    red = dst_r;
    green = dst_g;
    blue = dst_b;
    
    if (image == NULL) {
        fprintf(stderr, "Aucun fichier source transmis.\n");
        return 1;
    }
    if (header == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour le header.\n");
        return 1;
    }
    if (red == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour la couleur "
            "rouge.\n");
        return 1;
    }
    if (green == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour la couleur "
            "verte.\n");
        return 1;
    }
    if (blue == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis pour la couleur "
            "bleue.\n");
        return 1;
    }
    
    extract_header();
    extract_content(toHSV);
    
    return 0;
}

int merge_ppm(FILE* src_h, FILE* src_r, FILE* src_g, FILE* src_b, FILE* dst,
    int fromHSV) {
    int c, color = 0;
    int eof = 0;
    
    image = dst;
    header = src_h;
    red = src_r;
    green = src_g;
    blue = src_b;
    
    if (image == NULL) {
        fprintf(stderr, "Aucun fichier destination transmis.\n");
        return 1;
    }
    if (header == NULL) {
        fprintf(stderr, "Aucun fichier source transmis pour le header.\n");
        return 1;
    }
    if (red == NULL) {
        fprintf(stderr, "Aucun fichier source transmis pour la couleur "
            "rouge.\n");
        return 1;
    }
    if (green == NULL) {
        fprintf(stderr, "Aucun fichier source transmis pour la couleur "
            "verte.\n");
        return 1;
    }
    if (blue == NULL) {
        fprintf(stderr, "Aucun fichier source transmis pour la couleur "
            "bleue.\n");
        return 1;
    }
    
    while ((c = copy_char(header, image)) != EOF);
    c = 0;
    while (!eof) {
        c = fgetc(red);
        if (c == EOF) {
            eof = 1;
        }
        else {
            color = c << 16;
        }
        
        c = fgetc(green);
        if (c == EOF) {
            if (!eof) {
                fprintf(stderr, "Erreur lors de la lecture des fichiers de "
                    "couleurs. Les fichiers sont de tailles différentes.\n");
                return 1;
            }
            eof = 1;
        }
        else {
            if (eof) {
                fprintf(stderr, "Erreur lors de la lecture des fichiers de "
                    "couleurs. Les fichiers sont de tailles différentes.\n");
                return 1;
            }
            color |= c << 8;
        }
        
        c = fgetc(blue);
        if (c == EOF) {
            if (!eof) {
                fprintf(stderr, "Erreur lors de la lecture des fichiers de "
                    "couleurs. Les fichiers sont de tailles différentes.\n");
                return 1;
            }
            eof = 1;
        }
        else {
            if (eof) {
                fprintf(stderr, "Erreur lors de la lecture des fichiers de "
                    "couleurs. Les fichiers sont de tailles différentes.\n");
                return 1;
            }
            color |= c;
        }
        
        if (fromHSV)
            color = color_hsv_to_rgb(color);
        
        fputc(color >> 16 & 0xFF, image);
        fputc(color >> 8 & 0xFF,  image);
        fputc(color & 0xFF,       image);
    }
    
    return 0;
}
