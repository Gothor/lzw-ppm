#ifndef LZW_PPM_H_INCLUDED
#define LZW_PPM_H_INCLUDED

// Types
struct string {
    char* str;
    int length;
    int num;
};
typedef struct string string_t;

int lzw_ppm(FILE* src, FILE* dst);
int unlzw_ppm(FILE* src, FILE* dst);

#endif // LZW_PPM_H_INCLUDED
