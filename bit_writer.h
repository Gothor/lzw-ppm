#ifndef BIT_WRITER_H_INCLUDED
#define BIT_WRITER_H_INCLUDED

#ifdef DEBUG
    void print_buffer();
#endif

int write_bits(unsigned int v, int n, FILE* f);
void flush_buffer(FILE* f);
int read_bits(FILE* f, int n);

#endif // BIT_WRITER_H_INCLUDED