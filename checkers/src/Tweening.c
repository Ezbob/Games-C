
#include "Tweening.h"
#include <stdio.h>


void printRect(const SDL_Rect * r) {
    printf("RECT(x: %i, y: %i, w: %i, h: %i)\n", r->x, r->y, r->w, r->h);
}

void lerp(SDL_Rect * out, const SDL_Rect *start, const SDL_Rect *end, double f) {
    double t = 1.0 - f;
    out->x = start->x * t + end->x * f;
    out->y = start->y * t + end->y * f; 
}
