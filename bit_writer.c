#include <stdio.h>
#include <stdlib.h>
#include "bit_writer.h"

#ifdef DEBUG
static int  lus = 0;
#endif

static char buffer = 0;
static int  size = 0;

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
 * Ecrit les n bits de poids faible de la chaîne envoyee.
 */
int write_bits(unsigned int v, int n, FILE* f) {
    if (n > 32)
        return -1;
    
    // Si le buffer n'est pas vide, on finit de le remplir
    if (size + n >= 8) {
        buffer |= (char) (v >> (n - (8 - size)));
        fputc(buffer, f);
        #ifdef DEBUG
        fprintf(stderr, "---- ");
        print_buffer();
        #endif
        n -= (8 - size);
        size = 0;
        buffer = 0;
    }
    // Si ce qu'on doit ecrire ne remplit pas le buffer
    else {
        buffer |= v << (8 - (n + size));
        size += n;
        return 0;
    }
    
    // On ecrit les octets qui remplissent le buffer
    while (n >= 8) {
		buffer = (char) (v >> (n % 8 + 8 * (n / 8 - 1)));
        fputc(buffer, f);
        #ifdef DEBUG
        fprintf(stderr, "-------- ");
        print_buffer();
        #endif
        n -= 8;
    }
    
    if (n > 0) {
        buffer = (char) v << (8 - n);
        size = n;
    }
    else {
        buffer = 0;
    }
    
    #ifdef DEBUG
    fprintf(stderr, "-------- Reste : ");
    print_buffer();
    #endif
    
    return 0;
}

/**
 * Vide le buffer.
 */
void flush_buffer(FILE* f) {
    if (size != 0) {
        if (f != NULL)
            fputc(buffer, f);
        buffer = 0;
        size = 0;
        
        #ifdef DEBUG
        fprintf(stderr, "Vidage du buffer : ");
        print_buffer();
        #endif
    }
}

int read_bits(FILE* f, int n) {
    int code;
    int c;
    unsigned char b;
    #ifdef ADEBUG
    int i;
    #endif
    
    if (n > 32) {
        printf("Mais vous êtes fou !\n");
        return -1;
    }
    
    if (n < size) {
        code = buffer >> (8 - n);
        buffer <<= n;
        size -= n;
        return code;
    }
    
    code = (unsigned char) buffer >> (8 - size);
    #ifdef ADEBUG
    fprintf(stderr, "-- Code :       ");
    for (i = 31; i >= 0; i--)
        fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
    fprintf(stderr, "\n");
    #endif
    n -= size;
    while (n > 8) {
        c = fgetc(f);
        buffer = (char) c;
        #ifdef DEBUG
        fprintf(stderr, "%d char lus\n", ++lus);
        #endif
        if (c == EOF) {
            return -1;
        }
        code = (code << 8);
        #ifdef ADEBUG
        fprintf(stderr, "---- Code (1) : ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        code += (unsigned char) buffer;
        #ifdef ADEBUG
        fprintf(stderr, "---- Code (2) : ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        size = 8;
        n -= 8;
    }
    
    if (n > 0) {
        c = fgetc(f);
        buffer = (char) c;
        #ifdef DEBUG
        fprintf(stderr, "%d char lus\n", ++lus);
        #endif
        if (c == EOF) {
            return -1;
        }
        code = (code << n);
        #ifdef ADEBUG
        fprintf(stderr, "------ Code :   ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        b = 0;
        b |= buffer;
        b >>= 8 - n;
        code |= b;
        #ifdef ADEBUG
        fprintf(stderr, "------ Buffer >> (8 - %d) : ", n);
        for (i = 7; i >= 0; i--)
            fprintf(stderr, "%c", (buffer >> (8 - n)) & (1 << i) ? '1' : '0');
        fprintf(stderr, " (%d)\n", size);
        fprintf(stderr, "------ Code :   ");
        for (i = 31; i >= 0; i--)
            fprintf(stderr, "%c", code & (1 << i) ? '1' : '0');
        fprintf(stderr, "\n");
        #endif
        buffer <<= n;
        size = 8 - n;
    }
    
    #ifdef ADEBUG
    printf("------ Buffer : ");
    for (i = 7; i >= 0; i--)
        printf("%c", buffer & (1 << i) ? '1' : '0');
    printf(" (%d)\n", size);
    #endif
    
    return code;
}
