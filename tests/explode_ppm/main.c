#include <stdio.h>
#include <stdlib.h>
#include "../../ppm-exploder.h"

int main (int argc, char* argv[]) {
    int i;
    int opt = 0;
    FILE* src = NULL;
    FILE* dst = NULL;
    
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
                fprintf(stderr, "Usage: %s src\n", argv[0]);
                fprintf(stderr, "options:\n");
                fprintf(stderr, "\t-h prints this help\n");
                return -1;
            }
        }
    }
    
    if (argc - opt < 2) {
        fprintf(stderr, "Usage: %s src\n", argv[0]);
        return -1;
    }
    
    src = fopen(argv[1], "rb");
    if (src == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier source (%s).\n",
            argv[1]);
        return -1;
    }
    
    dst = fopen(".header", "wb");
    if (src == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier destination (%s).\n",
            ".header");
        return -1;
    }
    
    explode_ppm(src, dst, NULL, NULL, NULL);
    
    return 0;
}
