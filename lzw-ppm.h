#ifndef LZW_PPM_H_INCLUDED
#define LZW_PPM_H_INCLUDED

#ifdef DEBUG
    void print_buffer();
#endif

int write_bits(unsigned int v, int n);
void flush_buffer();
int lzw_ppm(char* src, char* dst);

#endif // LZW_PPM_H_INCLUDED