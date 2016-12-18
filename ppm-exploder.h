#ifndef PPM_EXPLODER_H_INCLUDED
#define PPM_EXPLODER_H_INCLUDED

enum encoding {
    UNKNOWN = 0,
    ASCII   = 3,
    BINARY  = 6
};
typedef enum encoding encoding_e;

int explode_ppm(FILE* src, FILE* dst_h, FILE* dst_r, FILE* dst_g, FILE* dst_b);
int merge_ppm(FILE* src_h, FILE* src_r, FILE* src_g, FILE* src_b, FILE* dst);

#endif // PPM_EXPLODER_H_INCLUDED
