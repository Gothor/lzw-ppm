#include <stdio.h>
#include <stdlib.h>
#include "lzw-ppm.h"

static char buffer = 0;
static int size = 0;

#ifdef DEBUG
    /**
     * Affiche le contenu du buffer.
     */
    void print_buffer() {
        int i;
        
        fprintf(stderr, "buffer: (");
        
        for (i = 0; i < 8; i++) {
            if (size == i)
                fprintf(stderr, "[");
            fprintf(stderr, "%c", buffer & (1 << (7 - i)) ? '1' : '0');
            if (i == 3)
                fprintf(stderr, "'");
        }
        fprintf(stderr, "])\n");
    }
#endif

/**
 * Ecrit les n bits de poids faible de la chaîne envoyée.
 */
int write_bits(unsigned int v, int n) {
    if (n > 32)
        return -1;
    
    // Si le buffer n'est pas vide, on finit de le remplir
    if (size + n > 8) {
        buffer |= (char) (v >> (n - (8 - size)));
        //fputc(buffer, f);
        n -= (8 - size);
        size = 0;
    }
    // Si ce qu'on doit écrire ne remplit pas le buffer
    else {
        buffer |= v << (8 - (n + size));
        size += n;
        return 0;
    }
    
    // On écrit les octets qui remplissent le buffer
    while (n >= 8) {
        buffer = (char) (v >> (n % 8 + 8 * (n / 8 - 1)));
        //fputc(buffer, f);
        n -= 8;
    }
    
    if (n > 0) {
        buffer = v << (8 - n);
        size = n;
    }
    
    return 0;
}

/**
 * Vide le buffer
 */

void flush_buffer() {
    buffer = 0;
    size = 0;
}