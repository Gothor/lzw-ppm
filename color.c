/**
 * @TODO correct hsv_to_rgb which does not work at all...
 */

#include <stdio.h>
#include <stdlib.h>
#include "color.h"

// Constantes
#define MASK_RED        (0xFF000000)
#define MASK_GREEN      (0x00FF0000)
#define MASK_BLUE       (0x0000FF00)

#define MASK_HUE        (0xFF000000)
#define MASK_SATURATION (0x00FF0000)
#define MASK_VALUE      (0x0000FF00)

#define MASK_ALPHA      (0x000000FF)

// Macros
#define RED(a)          (((a)->rgba & MASK_RED) >> 24)
#define GREEN(a)        (((a)->rgba & MASK_GREEN) >> 16)
#define BLUE(a)         (((a)->rgba & MASK_BLUE) >> 8)

#define HUE(a)          (((a)->hsva & MASK_HUE) >> 24)
#define SATURATION(a)   (((a)->hsva & MASK_SATURATION) >> 16)
#define VALUE(a)        (((a)->hsva & MASK_VALUE) >> 8)

#define ALPHA(a)        ((a)->rgba & MASK_ALPHA)

#define MAX(a, b)   ((a) > (b) ? (a) : (b))
#define MIN(a, b)   ((a) < (b) ? (a) : (b))  

static void rgb_to_hsv(color_t* c);
static void hsv_to_rgb(color_t* c);

color_t* color_create() {
    color_t* c = malloc(sizeof(*c));
    if (!c)
        return NULL;
    
    c->rgba = 0xFF;
    c->hsva = 0xFF;
    
    return c;
}

color_t* color_create_rgb(int r, int g, int b) {
    return color_create_rgba(r, g, b, 255);
}

color_t* color_create_rgba(int r, int g, int b, int a) {
    color_t* c = malloc(sizeof(*c));
    if (!c)
        return NULL;
    
    c->rgba = r << 24 | g << 16 | b << 8 | a;
    rgb_to_hsv(c);
    
    return c;
}

color_t* color_create_hsv(int h, int s, int v) {
    return color_create_rgba(h, s, v, 255);
}

color_t* color_create_hsva(int h, int s, int v, int a) {
    color_t* c = malloc(sizeof(*c));
    if (!c)
        return NULL;
    
    c->rgba = h << 24 | s << 16 | v << 8 | a;
    hsv_to_rgb(c);
    
    return c;
}

void color_set_rgb(color_t* c, int r, int g, int b) {
    c->rgba = r << 24 | g << 16 | b << 8 | ALPHA(c);
    rgb_to_hsv(c);
}

void color_set_rgba(color_t* c, int r, int g, int b, int a) {
    c->rgba = r << 24 | g << 16 | b << 8 | a;
    rgb_to_hsv(c);
}

void color_set_hsv(color_t* c, int h, int s, int v) {
    c->hsva = h << 24 | s << 16 | v << 8 | ALPHA(c);
    hsv_to_rgb(c);
}

void color_set_hsva(color_t* c, int h, int s, int v, int a) {
    c->hsva = h << 24 | s << 16 | v << 8 | a;
    hsv_to_rgb(c);
}

void color_print_rgb(color_t* c) {
    printf("rgb(%d, %d, %d)", RED(c), GREEN(c), BLUE(c));
}

void color_print_rgba(color_t* c) {
    printf("rgb(%d, %d, %d, %d)", RED(c), GREEN(c), BLUE(c), ALPHA(c));
}

void color_print_hsv(color_t* c) {
    printf("hsv(%d, %d, %d)", HUE(c), SATURATION(c), VALUE(c));
}

void color_print_hsva(color_t* c) {
    printf("hsv(%d, %d, %d, %d)", HUE(c), SATURATION(c), VALUE(c), ALPHA(c));
}

int color_red(color_t* c) {
    return RED(c);
}

int color_green(color_t* c) {
    return GREEN(c);
}

int color_blue(color_t* c) {
    return BLUE(c);
}

int color_hue(color_t* c) {
    return HUE(c);
}

int color_saturation(color_t* c) {
    return SATURATION(c);
}

int color_value(color_t* c) {
    return VALUE(c);
}

int color_alpha(color_t* c) {
    return ALPHA(c);
}

static void rgb_to_hsv(color_t* c) {
    int r = RED(c),
        g = GREEN(c),
        b = BLUE(c),
        max = MAX(r, MAX(g, b)),
        min = MIN(r, MIN(g, b)),
        h,
        s,
        v = max;
    
    if (max == min) {
        h = 0;
    }
    else if (max == r) {
        h = (((255 / 6) * (g - b)) / (max - min) + 255) % 255;
    }
    else if (max == g) {
        h = (((255 / 6) * (b - r)) / (max - min) + 255 / 3);
    }
    else if (max == b) {
        h = (((255 / 6) * (r - g)) / (max - min) + (255 * 2) / 3);
    }
    
    if (max == 0) {
        s = 0;
    }
    else {
        s = 255 - ((min * 255) / max);
    }
    
    c->hsva = h << 24 | s << 16 | v << 8 | ALPHA(c);
}

static void hsv_to_rgb(color_t* c) {
    int h = HUE(c),
        s = SATURATION(c),
        v = VALUE(c),
        hi = h / (255 / 6),
        f = h / (255 - 6) - hi,
        l = v * (255 - s),
        m = v * (255 - f * s),
        n = v * (255 - (255 - f) * s),
        r,
        g,
        b;
        
    switch(hi) {
        case 0 : r = v, g = n, b = l; break;
        case 1 : r = m, g = v, b = l; break;
        case 2 : r = l, g = v, b = n; break;
        case 3 : r = l, g = m, b = v; break;
        case 4 : r = n, g = l, b = v; break;
        case 5 : r = v, g = l, b = m; break;
    }
    
    c->rgba = r << 24 | g << 16 | b << 8 | ALPHA(c);
}
