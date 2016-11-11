#include <stdio.h>
#include <stdlib.h>

/* Options */
static int decomposed = 0;

int main (int argc, char* argv[]) {
    int i;
    int opt = 0;
    
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
    
    return 0;
}