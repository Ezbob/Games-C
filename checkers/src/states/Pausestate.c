
#include "Pausestate.h"
#include "Macros.h"
#include "Gamestate.h"
#include "States.h"
#include "SDL_ttf.h"
#include "Convert.h"
#include "Shared.h"

extern SDL_Renderer *g_renderer;
extern SDL_bool g_is_playing;
extern struct gt_Gamestate_Machine g_statemachine;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

SDL_Texture *ptext;
TTF_Font *font;
SDL_Rect quad;

SDL_bool pausestate_load(void) {
    font = TTF_OpenFont("assets/consola.ttf", 32);
    if ( !font ) {
        perror(TTF_GetError());
        return SDL_FALSE;
    }

    ptext = GT_LOAD_BLACK_TEXT("Paused", font);
    if ( !ptext ) {
        perror(SDL_GetError());
        return SDL_FALSE;
    }

    SDL_QueryTexture(ptext, NULL, NULL, &quad.w, &quad.h);
    quad.x = SCREEN_WIDTH / 2 - (quad.w / 2);
    quad.y = SCREEN_HEIGHT / 2 - (quad.h / 2);

    return SDL_TRUE;
}

void pausestate_unload(void) {
    TTF_CloseFont(font);
    SDL_DestroyTexture(ptext);
}

void pausestate_render(void) {
    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE);
    SDL_RenderClear(g_renderer);

    SDL_RenderCopy(g_renderer, ptext, NULL, &quad);
    SDL_RenderPresent(g_renderer);
}

void pausestate_update(void) {}


void pausestate_handleKeyState(const Uint8 *keyState) {
    if ( keyState[SDL_SCANCODE_RETURN] ) {
        gt_gsmachine_goToState(&g_statemachine, GT_BOARD_STATE);
    }
}

void pausestate_handleEvent(const SDL_Event *event) {
    switch (event->type) {
        case SDL_QUIT:
            g_is_playing = SDL_FALSE;
            break;
        default:
            break;
    }
}
