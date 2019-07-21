#ifndef _HEADER_FILE_Gamestate_20190718184924_
#define _HEADER_FILE_Gamestate_20190718184924_

#include "sdl2/SDL.h"

typedef void (*gs_statefunc)(void);
typedef SDL_bool (*gs_boolstatefunc)(void);

struct gt_Gamestate {
    SDL_bool stopped;
    gs_statefunc update;
    gs_statefunc render;
    gs_boolstatefunc load; 
};

struct gt_Gamestate_Machine {
    struct gt_Gamestate **states;
    int current_state_index;
};

#ifndef GT_NUMBER_OF_STATES
    #define GT_NUMBER_OF_STATES 1
#endif

/* All states are externally defined in another TU */
extern struct gt_Gamestate *GT_GAMESTATES[GT_NUMBER_OF_STATES + 1];

/*
    Sentry state (the null state) defined here to give it
    a static address 
*/
static struct gt_Gamestate GT_ENDSTATE = ((struct gt_Gamestate){
    .stopped = SDL_FALSE,
    .update = NULL,
    .render = NULL,
    .load = NULL,
}); 

/* Game Machine function api */

SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *);
struct gt_Gamestate *gt_gsmachine_advanceState(struct gt_Gamestate_Machine *);
SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *);


/*
    Syntatic sugar macros for defining the state array
 */
#define GT_END_OF_STATE_ARRAY (&GT_ENDSTATE)
#define GT_GAME_STATE_ARRAY struct gt_Gamestate *GT_GAMESTATES[GT_NUMBER_OF_STATES + 1]

#define GT_IS_VALID_STATE(state) (state != (&GT_ENDSTATE))

#endif