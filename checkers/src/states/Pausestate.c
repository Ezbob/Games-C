
#include "Pausestate.h"
#include "../Macros.h"
#include "../Gamestate.h"

extern SDL_Renderer *g_renderer;
extern SDL_bool g_is_playing;

struct gt_Gamestate gs_pausestate;

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
    if (keyState[SDL_SCANCODE_RETURN])
        gs_pausestate.stopped = SDL_TRUE;
}

void pausestate_handleEvent(const SDL_Event *event) {
    UNUSED(event);
}

struct gt_Gamestate gs_pausestate = GT_SETUP_GAMESTATE(pausestate);