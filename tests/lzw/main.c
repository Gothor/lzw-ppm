#include <stdio.h>
#include <stdlib.h>
#include "../../lzw-ppm.h"

/* Options */
static int decomposed = 0;

int main (int argc, char* argv[]) {
    int i;
    int opt = 0;
    FILE* source_file = NULL;
    FILE* destination_file = NULL;
    
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
                fprintf(stderr, "Usage: lzw-ppm [options] src [dst]\n");
                fprintf(stderr, "options:\n");
                fprintf(stderr, "\t-h prints this help\n");
                fprintf(stderr, "\t-d to specify a decomposed image (header: <image>.hppm, red: <image>.rppm, green: <image.rppm>, blue: <image.rppm>)\n");
                return -1;
            case 'd':
                decomposed = 1;
                break;
            }
        }
    }
    
    if (argc - opt < 2) {
        fprintf(stderr, "Usage: lzw-ppm [options] src [dst]\n");
        return -1;
    }

    // Compression
    
    source_file = fopen(argv[argc - opt - 1], "r");
    if (source_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier source (%s).\n", argv[argc - opt - 1]);
        return -1;
    }
    
    destination_file = fopen(".compressed", "w");
    if (destination_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier destination (%s).\n", ".compressed");
        return -1;
    }
       
    lzw_ppm(source_file, destination_file);
    printf("Compression terminée dans le fichier \".compressed\".\n");
    
    fclose(destination_file);
    fclose(source_file);

    // Décompression

    source_file = fopen(".compressed", "r");
    if (source_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier source (%s).\n", ".compressed");
        return -1;
    }
    
    destination_file = fopen(".uncompressed", "w");
    if (destination_file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier destination (%s).\n", ".uncompressed");
        return -1;
    }
       
    unlzw_ppm(source_file, destination_file);
    printf("Décompression terminée dans le fichier \".uncompressed\".\n");
    
    fclose(destination_file);
    fclose(source_file);

    return 0;
}
