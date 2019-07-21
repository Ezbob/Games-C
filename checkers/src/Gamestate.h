#ifndef _HEADER_FILE_Gamestate_20190718184924_
#define _HEADER_FILE_Gamestate_20190718184924_

#include "sdl2/SDL.h"

typedef void (*gs_statefunc)(void);
typedef SDL_bool (*gs_boolstatefunc)(void);
typedef void (*gs_eventstatefunc)(const SDL_Event *);
typedef void (*gs_keystatefunc)(const Uint8 *);

struct gt_Gamestate {
    SDL_bool stopped;
    gs_eventstatefunc handleEvent;
    gs_keystatefunc handleKeyState;
    gs_boolstatefunc load; 
    gs_statefunc update;
    gs_statefunc render;
};

struct gt_Gamestate_Machine {
    struct gt_Gamestate **states;
    int current_state_index;
};

/*
    Sentry state (the null state) defined here to give it
    a static address 
*/
static struct gt_Gamestate GT_ENDSTATE = ((struct gt_Gamestate){
    .stopped = SDL_TRUE
}); 

/* Game Machine function api */

SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *);
struct gt_Gamestate *gt_gsmachine_advanceState(struct gt_Gamestate_Machine *);
SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *, struct gt_Gamestate **);

void gt_gstate_pump_events(struct gt_Gamestate *);

#define GT_STATE_ARRAY_END (&GT_ENDSTATE)


#endif