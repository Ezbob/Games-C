
#include "Convert.h"

SDL_Texture *convert_to_texture(SDL_Renderer *renderer, SDL_Surface *surface) {
    if (!surface) return NULL;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) return NULL;

    SDL_FreeSurface(surface);
    return texture;
}
