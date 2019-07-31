
#include "Tweening.h"

void lerp(SDL_Rect * out, const SDL_Rect *r, double f) {
    double t = 1.0 - f;
    out->x = out->x * t + r->x * f;
    out->y = out->y * t + r->y * f; 
}
