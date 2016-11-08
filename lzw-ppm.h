#ifndef LZW_PPM_H_INCLUDED
#define LZW_PPM_H_INCLUDED

enum encoding {
    UNKNOWN = 0,
    ASCII   = 3,
    BINARY  = 6
};
typedef enum encoding encoding_e;

int  write_bits(unsigned int v, int n);
void flush_buffer();
int  lzw_ppm(char* src, char* dst);

#ifdef DEBUG
    void print_buffer();
#endif

#endif // LZW_PPM_H_INCLUDED