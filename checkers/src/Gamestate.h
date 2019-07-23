#ifndef _HEADER_FILE_Gamestate_20190718184924_
#define _HEADER_FILE_Gamestate_20190718184924_

#include "sdl2/SDL.h"

typedef void (*gs_statefunc)(void);
typedef SDL_bool (*gs_boolstatefunc)(void);
typedef void (*gs_eventstatefunc)(const SDL_Event *);
typedef void (*gs_keystatefunc)(const Uint8 *);

struct gt_Gamestate {
    SDL_bool stopped;
    SDL_bool isLoaded;
    gs_eventstatefunc handleEvent;
    gs_keystatefunc handleKeyState;
    gs_boolstatefunc load; 
    gs_statefunc update;
    gs_statefunc render;
};

struct gt_Gamestate_Machine {
    SDL_bool shouldSkip;
    struct gt_Gamestate **states;
    int next_state_index;
};

/* Game Machine function api */

SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *m);
struct gt_Gamestate *gt_gsmachine_advanceState(struct gt_Gamestate_Machine *m);
SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *m, struct gt_Gamestate **s);
int gt_gsmachine_getCurrentStateIndex(struct gt_Gamestate_Machine *m);
void gt_gsmachine_setCurrentStateIndex(struct gt_Gamestate_Machine *m, int i);
void gt_gsmachine_goToState(struct gt_Gamestate_Machine *m, int i);
struct gt_Gamestate *gt_gsmachine_setupSkip(struct gt_Gamestate_Machine *m);

void gt_gstate_pump_events(struct gt_Gamestate *m);

#define GT_STATE_ARRAY_END NULL

#endif