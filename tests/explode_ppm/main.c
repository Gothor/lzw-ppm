#include <stdio.h>
#include <stdlib.h>
#include "../../ppm-exploder.h"

#define NB_DST_FILES    4

static int toHSV = 0;

int main (int argc, char* argv[]) {
    int i;
    int opt = 0;
    int failed = 0;
    FILE* src = NULL;
    FILE* dst[NB_DST_FILES] = {NULL, NULL, NULL, NULL};
    FILE* dst_merge = NULL;
    char* filenames[4] = {".header", ".red", ".green", ".blue"};

    // Des options ?
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
                fprintf(stderr, "Usage: %s [options] src\n", argv[0]);
                fprintf(stderr, "options:\n");
                fprintf(stderr, "\t-h prints this help\n");
                fprintf(stderr, "\t-m convert colors to HSV\n");
                return -1;
            case 'm':
                toHSV = 1;
                break;
            }
        }
    }
    
    if (argc - opt < 2) {
        fprintf(stderr, "Usage: %s [options] src\n", argv[0]);
        return -1;
    }
    
    // On ouvre nos gentils fichiers
    src = fopen(argv[argc - 1], "rb");
    if (src == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier source (%s).\n",
            argv[argc - 1]);
        return -1;
    }
    
    for (i = 0; i < NB_DST_FILES; i++) {
        dst[i] = fopen(filenames[i], "wb");
        if (dst[i] == NULL) {
            fprintf(stderr, "Impossible d'ouvrir le fichier destination (%s)."
                "\n", filenames[i]);
            return -1;
        }
    }
    
    // On bosse
    explode_ppm(src, dst[0], dst[1], dst[2], dst[3], toHSV);
    
    // Et on les referme
    fclose(src);
    for (i = 0; i < NB_DST_FILES; i++) {
        fclose(dst[i]);
    }
    
    if (!failed) {
        // On ouvre ceux dont on a besoin
        for (i = 0; i < NB_DST_FILES; i++) {
            dst[i] = fopen(filenames[i], "rb");
            if (dst[i] == NULL) {
                fprintf(stderr, "Impossible d'ouvrir le fichier source (%s).\n",
                    filenames[i]);
                return -1;
            }
        }
        
        dst_merge = fopen(".output.ppm", "wb");
        if (dst_merge == NULL) {
            fprintf(stderr, "Impossible d'ouvrir le fichier destination (%s) du "
                "merge.\n", ".output");
            return -1;
        }
        
        merge_ppm(dst[0], dst[1], dst[2], dst[3], dst_merge);
        
        // Et on les referme
        fclose(dst_merge);
    }
    
    return 0;
}
