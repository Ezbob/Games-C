#ifndef _HEADER_FILE_Gamestate_20190718184924_
#define _HEADER_FILE_Gamestate_20190718184924_

#include "sdl2/SDL.h"

typedef void (*gs_statefunc)(void *state_data);
typedef SDL_bool (*gs_boolstatefunc)(void *state_data);

struct gt_Gamestate {
    int state_index;
    gs_statefunc update;
    gs_statefunc render;
    gs_boolstatefunc load; 
};

struct gt_Gamestate_Machine {
    struct gt_Gamestate **states;
    int current_state_index;
};

extern struct gt_Gamestate_Macine *GT_GAMESTATES;
static const struct gt_Gamestate GT_ENDSTATE = {0}; 

SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *);
struct gt_Gamestate *gt_gsmachine_getNextState(struct gt_Gamestate_Machine *);
SDL_bool gt_gsmachine_initialize(struct gt_Gamestate_Machine *);

#endif