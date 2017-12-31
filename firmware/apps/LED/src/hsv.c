#include "hsv.h"
#include <math.h>

void hsv_to_rgb(float h, float s, float v, uint32_t *rgb)
{
    int i;
    float f,p,q,t;

    uint8_t r,g,b;

    h = fmax(0.0, fmin(360.0, h));
    s = fmax(0.0, fmin(100.0, s));
    v = fmax(0.0, fmin(100.0, v));

    s /= 100;
    v /= 100;

    if(s == 0) {
        // Achromatic (grey)
        r = g = b = round(v*255);
        return;
    }

    h /= 60; // sector 0 to 5
    i = floor(h);
    f = h - i; // factorial part of h
    p = v * (1 - s);
    q = v * (1 - s * f);
    t = v * (1 - s * (1 - f));

    switch(i) {
        case 0:
            r = round(255*v);
            g = round(255*t);
            b = round(255*p);
            break;
        case 1:
            r = round(255*q);
            g = round(255*v);
            b = round(255*p);
            break;
        case 2:
            r = round(255*p);
            g = round(255*v);
            b = round(255*t);
            break;
        case 3:
            r = round(255*p);
            g = round(255*q);
            b = round(255*v);
            break;
        case 4:
            r = round(255*t);
            g = round(255*p);
            b = round(255*v);
            break;
        default: // case 5:
            r = round(255*v);
            g = round(255*p);
            b = round(255*q);
    }

    *rgb = (r << 16) | (g << 8) | b;
}
