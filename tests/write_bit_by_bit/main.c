#include <stdio.h>
#include <stdlib.h>
#include "../../lzw-ppm.h"

enum values_range {
    LOW = 0,
    AVERAGE = 128,
    HIGH = 256
};
typedef enum values_range values_range;

values_range from = LOW;

void print_ten_buffer_values(int v) {
    int i, j;
    int max = 1;
    int nbBits = 1;
    while (v > max) {
        max <<= 1;
        max++;
        nbBits++;
    }
    
    print_buffer();
    for (i = 0; i < 10; i++, v++) {
        if (v > max) {
            max <<= 1;
            max++;
            nbBits++;
        }
        printf("Ecrire %d (", v);
        for (j = nbBits - 1; j >= 0; j--) {
            printf("%c", v & (1 << j) ? '1' : '0');
        }
        printf(") :\n");
        write_bits(v, nbBits);
        print_buffer();
    }
}

int main (int argc, char* argv[]) {
    int i;
    int opt = 0;
    
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
                fprintf(stderr, "Usage: write_bit_by_bit [options]\n");
                fprintf(stderr, "options:\n");
                fprintf(stderr, "\t-h prints this help\n");
                fprintf(stderr, "\t-l to fill the buffer with low values (%d-%d)\n", LOW, LOW + 9);
                fprintf(stderr, "\t-a to fill the buffer with average values (%d-%d)\n", AVERAGE, AVERAGE + 9);
                fprintf(stderr, "\t-H to fill the buffer with high values (%d-%d)\n", HIGH, HIGH + 9);
                return -1;
            case 'l':
                from = LOW;
                break;
            case 'a':
                from = AVERAGE;
                break;
            case 'H':
                from = HIGH;
                break;
            }
        }
    }
    
    print_ten_buffer_values(from);
    
    return 0;
}