#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "Macros.h"
#include "Gameclock.h"
#include "Gamestate.h"
#include "States.h"

#if defined(_WIN32)
    #define MAIN_NAME WinMain
#else
    #define MAIN_NAME main
#endif

extern struct gt_Gamestate *gt_gamestates[GT_NUMBER_OF_STATES];

static const double MS_PER_UPDATE = 16.0;
static const int SCREEN_WIDTH = 840;
static const int SCREEN_HEIGHT = 860;

static const char *g_window_title = "Pure C Checkers";
SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_bool g_is_playing = SDL_TRUE;

struct gt_Gameclock g_gameclock;
struct gt_Gamestate_Machine g_statemachine;


void initGlobalData() {
    gt_gameclock_init(&g_gameclock, MS_PER_UPDATE);
    gt_gsmachine_init(&g_statemachine, gt_gamestates);
}

void unloadData(struct gt_Gamestate_Machine *m) {
    for (int i = 0; i < (GT_NUMBER_OF_STATES - 1); ++i) {
        struct gt_Gamestate *state = m->states[i];
        if (state->isLoaded) state->unload();
    }
}

SDL_bool sdlInit() {
    if ( SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT,
                                     SDL_WINDOW_SHOWN, &g_window, &g_renderer) == -1 )
        goto error_label;

    if ( SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE) == -1 )
        goto error_label;

    if ( SDL_RenderClear(g_renderer) == -1 )
        goto error_label;

    SDL_SetWindowTitle(g_window, g_window_title);

    return SDL_TRUE;

error_label:
    perror(SDL_GetError());
    return SDL_FALSE;
}

void sdlDestroy() {
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
}

int MAIN_NAME(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    if ( !sdlInit() ) {
        return EXIT_FAILURE;
    }

    initGlobalData();

    struct gt_Gamestate *state = gt_gsmachine_advanceState(&g_statemachine);

    if ( state == GT_STATE_ARRAY_END ) {
        perror("No valid start game state");
        goto initialized_failure;
    }

    while ( g_is_playing && state != GT_STATE_ARRAY_END ) {

        if ( !state->isLoaded && !(state->isLoaded = state->load()) ) {
            perror("Could not initialize state");
            goto initialized_failure;
        }

        while ( g_is_playing && !g_statemachine.shouldSkip ) {

            gt_gstate_pump_events(state);

            while ( GT_CLOCK_SHOULD_UPDATE(g_gameclock) ) {
                state->update();

                GT_CLOCK_LAG_UPDATE(g_gameclock);
            }

            state->render();

            GT_CLOCK_TICK(g_gameclock);
        }

        state = gt_gsmachine_setupSkip(&g_statemachine);
    }

    unloadData(&g_statemachine);
    sdlDestroy();
    return EXIT_SUCCESS;

initialized_failure:
    sdlDestroy();
    return EXIT_FAILURE;
}
