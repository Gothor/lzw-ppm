#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

struct color {
    int rgba;
    int hsva;
};
typedef struct color color_t;

color_t* color_create();
color_t* color_create_rgb(int r, int g, int b);
color_t* color_create_rgba(int r, int g, int b, int a);
color_t* color_create_hsv(int h, int s, int b);
color_t* color_create_hsva(int h, int s, int b, int a);

void color_set_rgb(color_t* c, int r, int g, int b);
void color_set_rgba(color_t* c, int r, int g, int b, int a);
void color_set_hsv(color_t* c, int h, int s, int v);
void color_set_hsva(color_t* c, int h, int s, int v, int a);

void color_print_rgb(color_t* c);
void color_print_rgba(color_t* c);
void color_print_hsv(color_t* c);
void color_print_hsva(color_t* c);

int color_red(color_t* c);
int color_green(color_t* c);
int color_blue(color_t* c);
int color_hue(color_t* c);
int color_saturation(color_t* c);
int color_value(color_t* c);
int color_alpha(color_t* c);

int color_rgb_to_hsv(int c);

#endif // COLOR_H_INCLUDED