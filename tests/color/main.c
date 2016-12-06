#include <stdio.h>
#include <stdlib.h>
#include "../../color.h"

void show_infos(color_t* c) {
    printf("c = ");
    color_print_rgb(c);
    printf("\n"
           "c = ");
    color_print_hsv(c);
    printf("\n"
           "alpha = %d\n\n", color_alpha(c));
}

int main(int argc, char* argv[]) {
    int opt = 0;
    color_t* c;
    int i, j, k;
    
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
            }
        }
    }
    
    if (argc == 4) {
        i = atoi(argv[1]);
        j = atoi(argv[2]);
        k = atoi(argv[3]);
        
        c = color_create_rgb(i, j, k);
        show_infos(c);
        
        color_set_hsv(c, i, j, k);
        show_infos(c);
        
        free(c);
        
        return 0;
    }
    
    // From RGB
    
    // hsv(21, 255, 200)
    c = color_create_rgb(200, 100, 0);
    show_infos(c);
    
    // hsv(64, 255, 200)
    color_set_rgb(c, 100, 200, 0);
    show_infos(c);
    
    // hsv(106, 255, 200)
    color_set_rgb(c, 0, 200, 100);
    show_infos(c);
    
    // hsv(149, 255, 200)
    color_set_rgb(c, 0, 100, 200);
    show_infos(c);
    
    // hsv(235, 255, 200)
    color_set_rgb(c, 200, 0, 100);
    show_infos(c);
    
    // hsv(191, 255, 200)
    color_set_rgb(c, 100, 0, 200);
    show_infos(c);
    
    // hsv(0, 0, 0)
    color_set_rgb(c, 0, 0, 0);
    show_infos(c);
    
    // hsv(0, 0, 255)
    color_set_rgb(c, 255, 255, 255);
    show_infos(c);
    
    // From HSV
    
    // rgb(0, 0, 0)
    color_set_hsv(c, 200, 100, 0);
    show_infos(c);
    
    // rgb(0, 0, 0)
    color_set_hsv(c, 100, 200, 0);
    show_infos(c);
    
    // rgb(100, 22, 22)
    color_set_hsv(c, 0, 200, 100);
    show_infos(c);
    
    // rgb(200, 122, 122)
    color_set_hsv(c, 0, 100, 200);
    show_infos(c);
    
    // rgb(100, 100, 100)
    color_set_hsv(c, 200, 0, 100);
    show_infos(c);
    
    // rgb(200, 200, 200)
    color_set_hsv(c, 100, 0, 200);
    show_infos(c);
    
    // rgb(0, 0, 0)
    color_set_hsv(c, 0, 0, 0);
    show_infos(c);
    
    // rgb(255, 0, 0)
    color_set_hsv(c, 255, 255, 255);
    show_infos(c);
    
    free(c);
    
    return 0;
}