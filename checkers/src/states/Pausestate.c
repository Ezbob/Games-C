
#include "Pausestate.h"
#include "../Macros.h"
#include "../Gamestate.h"
#include "../States.h"

extern SDL_Renderer *g_renderer;
extern SDL_bool g_is_playing;
extern struct gt_Gamestate_Machine g_statemachine;

SDL_bool pausestate_load(void) {

    return SDL_TRUE;
}

void pausestate_render(void) {
    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE);
    SDL_RenderClear(g_renderer);

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

