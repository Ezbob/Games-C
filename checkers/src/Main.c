#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "Macros.h"
#include "Gameclock.h"
#include "Gamestate.h"
#include "States.h"
#include "SDL_ttf.h"
#include "Constants.h"

#if defined(_WIN32)
    #define MAIN_NAME WinMain
#else
    #define MAIN_NAME main
#endif

extern struct gt_Gamestate *gt_gamestates[GT_NUMBER_OF_STATES];

static const char *g_window_title = "Pure C Checkers";
SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_bool g_is_playing = SDL_TRUE;

struct gt_Gameclock g_gameclock;
struct gt_Gamestate_Machine g_statemachine;

void initGlobalData() {
    gt_gameclock_init(&g_gameclock, MS_PER_UPDATE);
    gt_gsmachine_init(&g_statemachine, &g_gameclock, gt_gamestates);
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

    if ( TTF_Init() == -1 ) {
        goto error_label;
    }

    SDL_SetWindowTitle(g_window, g_window_title);

    return SDL_TRUE;

error_label:
    perror(SDL_GetError());
    return SDL_FALSE;
}

void sdlDestroy() {
    TTF_Quit();
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

    int exitcode = gt_gsmachine_runloop(&g_statemachine);

    switch (exitcode) {
        case 1:
            goto initialized_failure;
        case 2:
            goto unload_initialized_failure;
        default:
            break;
    }

    unloadData(&g_statemachine);
    sdlDestroy();
    return EXIT_SUCCESS;

unload_initialized_failure:
    unloadData(&g_statemachine);
initialized_failure:
    sdlDestroy();
    return EXIT_FAILURE;
}
