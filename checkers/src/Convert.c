
#include "Convert.h"

SDL_Texture *convert_from_texture(SDL_Renderer *renderer, SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if ( !texture ) {
        return NULL;
    }
    SDL_FreeSurface(surface);
    return texture;
}
