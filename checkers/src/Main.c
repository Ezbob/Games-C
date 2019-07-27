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
static const char *g_window_title = "Checkers";

SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;

struct gt_Gameclock g_gameclock;
struct gt_Gamestate_Machine g_statemachine;

void initGlobalData() {
    gt_gameclock_init(&g_gameclock, MS_PER_UPDATE);
    gt_gsmachine_init(&g_statemachine, &g_gameclock, gt_gamestates, GT_NUMBER_OF_STATES);
}

SDL_bool sdlInit() {
    if ( !(g_window = SDL_CreateWindow(g_window_title, SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) )
        goto sdl_error;

    if ( !(g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED)) )
        goto sdl_error;

    if ( SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE) == -1 )
        goto sdl_error;

    if ( SDL_RenderClear(g_renderer) == -1 )
        goto sdl_error;

    if ( TTF_Init() == -1 ) {
        goto TTF_error;
    }

    SDL_SetWindowTitle(g_window, g_window_title);

    return SDL_TRUE;

TTF_error:
    perror(TTF_GetError());
    return SDL_FALSE;

sdl_error:
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

    switch (gt_gsmachine_runLoop(&g_statemachine)) {
        case 1:
            goto initialized_failure;
        case 2:
            goto unload_initialized_failure;
        default:
            break;
    }

    gt_gsmachine_unloadAll(&g_statemachine);
    sdlDestroy();
    return EXIT_SUCCESS;

unload_initialized_failure:
    gt_gsmachine_unloadAll(&g_statemachine);
initialized_failure:
    sdlDestroy();
    return EXIT_FAILURE;
}
