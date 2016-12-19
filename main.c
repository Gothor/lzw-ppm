#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "lzw.h"

/* Options */
static int uncompress = 0;
static int splitted = 0;
static int color_mode_hsv = 0;
static int given_dst_name = 0;

int confirm() {
    int c = fgetc(stdin);
    while(c != 'y' && c != 'Y' && c != 'n' && c != 'N') {
        fflush(stdin);
        c = fgetc(stdin);
    }
    if (c == 'y' || c == 'Y')
        return 1;
    return 0;
}

char* remove_extension(const char* s) {
    int i, e = -1;
    char* res = NULL;
    
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] == '.') e = i;
    }
    if (e == -1) e = i - 1;
    
    res = malloc(sizeof(*res) * (e + 1));
    assert(res);
    
    for (i = 0; i < e; i++)
        res[i] = s[i];
    res[e] = '\0';
    
    return res;
}

void print_usage() {
    fprintf(stderr, "Usage: lzw-ppm [options] src_name [dst_name]\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "\t-h prints this help\n");
    fprintf(stderr, "\t-u to uncompress instead of compress src_name\n");
    fprintf(stderr, "\t-s to split the image before compression or to specify a"
        " splitted image image before decompression\n");
    fprintf(stderr, "\t-c to convert into HSV mode when splitting or to convert"
        " from HSV mode when merging\n");
}

int file_exists(char* s) {
    FILE* f = NULL;
    f = fopen(s, "r");
    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
}

int main (int argc, char* argv[]) {
    int i;
    int opt = 0;
    char* src_name = NULL;
    char* dst_name[5] = {NULL, NULL, NULL, NULL, NULL};
    FILE* src;
    FILE* dst[4] = {NULL, NULL, NULL, NULL};
    
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
                print_usage();
                return -1;
            case 'u':
                uncompress = 1;
                break;
            case 's':
                splitted = 1;
                break;
            case 'c':
                color_mode_hsv = 1;
                break;
            }
        }
    }
    
    if (argc - opt < 2 || argc - opt > 3) {
        print_usage();
        return -1;
    }
    
    src_name = argv[opt + 1];
    if (argc - opt == 3) {
        given_dst_name = 1;
        dst_name[0] = argv[opt + 2];
    }
    else {
        dst_name[0] = remove_extension(src_name);
    }
    
    // Splitted source/destination
    if (splitted) {
        
    }
    // Not splitted source/destination
    else {
        // On ouvre la source
        src = fopen(src_name, "rb");
        if (src == NULL) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier source %s."
                "\n", src_name);
            free(dst_name[0]);
            return -1;
        }
        
        // On nomme la destination correctement
        if (!given_dst_name) {
            if (uncompress)
                strcat(dst_name[0], ".ppm");
            else
                strcat(dst_name[0], ".xppm");
        }
        
        printf("Source : %s\n", src_name);
        printf("Destination : %s\n", dst_name[0]);
        
        // On vérifie que le fichier n'existe pas déjà (ce serait bête...)
        if (file_exists(dst_name[0])) {
            printf("Le fichier %s existe déjà. Êtes-vous sûr de vouloir le "
                "remplacer ? [Y/N] ", dst_name[0]);
            if (!confirm()) {
                free(dst_name[0]);
                fclose(src);
                return -1;
            }
        }
        
        // On ouvre la destination
        dst[0] = fopen(dst_name[0], "wb");
        if (dst[0] == NULL) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier destination "
                "%s.\n", dst_name[0]);
            free(dst_name[0]);
            return -1;
        }
        
        // On bosse (oui, on dirait pas... Mais si... on bosse...)
        if (uncompress) {
            printf("Décompression de %s en cours...\n", src_name);
            unlzw_ppm(src, dst[0]);
        }
        else {
            printf("Compression de %s en cours...\n", src_name);
            lzw_ppm(src, dst[0]);
        }
        
        // On ferme ! Revenez demain !
        fclose(src);
        fclose(dst[0]);
    }
    
    for (i = 0; i < 5; i++)
        if (dst_name[i] != NULL && (i != 0 || !given_dst_name))
            free(dst_name[i]);
    
    return 0;
}
