#include <stdio.h>
#include <stdlib.h>
#include "../../lzw-ppm.h"

/* Options */
static int lzw = 1;

int main (int argc, char* argv[]) {
    int i;
    int opt = 0;
    FILE* source_file = NULL;
    FILE* destination_file = NULL;
    char* source_name = NULL;
    
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
                fprintf(stderr, "Usage: lzw-ppm [options] src\n");
                fprintf(stderr, "options:\n");
                fprintf(stderr, "\t-h prints this help\n");
                fprintf(stderr, "\t-u to uncompress src\n");
                return -1;
            case 'u':
                lzw = 0;
                break;
            }
        }
    }
    
    if (argc - opt < 2) {
        fprintf(stderr, "Usage: lzw-ppm [options] src\n");
        return -1;
    }
    
    source_name = argv[argc - 1];

    // Ouverture du fichier src
    
    source_file = fopen(source_name, "rb");
    if (source_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier source (%s).\n",
            source_name);
        return -1;
    }
    
    // Ouverture du fichier dst
    
    if (lzw)
        destination_file = fopen(".compressed", "wb");
    else
        destination_file = fopen(".uncompressed.ppm", "wb");
    if (destination_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier destination (%s).\n",
            lzw ? ".compressed" : ".uncompressed.ppm");
        return -1;
    }
    
    // Compression
    if (lzw) {       
        lzw_ppm(source_file, destination_file);
        printf("Compression terminée dans le fichier \".compressed\".\n");
    }
    // Décompression
    else {
        unlzw_ppm(source_file, destination_file);
        printf("Décompression terminée dans le fichier "
            "\".uncompressed.ppm\".\n");
    }
    
    // Fermeture des fichiers
    fclose(destination_file);
    fclose(source_file);

    return 0;
}
