
#include "Gameoverstate.h"
#include "sdl2/SDL_ttf.h"
#include "Convert.h"
#include "Shared.h"
#include "Macros.h"
#include "States.h"

extern SDL_Renderer *g_renderer;
extern SDL_bool g_is_playing;
extern struct gt_Gamestate_Machine g_statemachine;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

TTF_Font *font;

struct Text {
    SDL_Texture *texture;
    SDL_Rect position;
};

struct Text text[2];

SDL_bool gameoverstate_load(void) {
    font = TTF_OpenFont("assets/consola.ttf", 32);
    int w, h;

    if (!font) {
        perror(TTF_GetError());
        return SDL_FALSE;
    }

    text[0].texture = GT_LOAD_BLACK_TEXT("Gameover", font);
    if (!text[0].texture)
        goto sdl_error_state;

    SDL_QueryTexture(text[0].texture, NULL, NULL, &w, &h);
    text[0].position.x = SCREEN_WIDTH / 2;
    text[0].position.y = SCREEN_HEIGHT / 2;
    text[0].position.h = h;
    text[0].position.w = w;

    text[1].texture = GT_LOAD_BLACK_TEXT("SOMEBODY WON", font);
    if (!text[1].texture)
        goto sdl_error_state;

    SDL_QueryTexture(text[1].texture, NULL, NULL, &w, &h);
    text[1].position.x = SCREEN_WIDTH / 2;
    text[1].position.y = SCREEN_HEIGHT / 2 + (h + 4);
    text[1].position.h = h;
    text[1].position.w = w;

    return SDL_TRUE;

sdl_error_state:
    perror(SDL_GetError());
    return SDL_FALSE;
}

void gameoverstate_unload(void) {
    SDL_DestroyTexture(text[0].texture);
    SDL_DestroyTexture(text[1].texture);
}

void gameoverstate_render(void) {
    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE);
    SDL_RenderClear(g_renderer);

    SDL_RenderCopy(g_renderer, text[0].texture, NULL, &text[0].position);
    SDL_RenderCopy(g_renderer, text[1].texture, NULL, &text[1].position);
    SDL_RenderPresent(g_renderer);
}

void gameoverstate_update(void) {

}

void gameoverstate_handleKeyState(const Uint8 *ks) {
    if ( ks[SDL_SCANCODE_ESCAPE] ) {
        gt_gsmachine_goToState(&g_statemachine, GT_END_STATE);
    }
}

void gameoverstate_handleEvent(const SDL_Event *e) {
    switch (e->type) {
        case SDL_QUIT:
            g_is_playing = SDL_FALSE;
            break;
        default:
            break;
    }
}


