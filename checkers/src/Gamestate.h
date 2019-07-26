/**
 * Gamestate: The Gamestate runtime machine.
 * This file adds a gamestate runtime machine, such that a game can be split up in
 * different gamestates. These gamestates can then be switched in and out via a goto function
 * defined on the machine.
 * 
 * It is important to note that the gamemachine does not own the collection of the different states
 * that is can switch between; rather it expects it to be defined elsewhere. This has the 
 * implication that the gamemachine does not attempt to clean up the state when it is 
 * done executing.
 */
#ifndef _HEADER_FILE_Gamestate_20190718184924_
#define _HEADER_FILE_Gamestate_20190718184924_

#include "sdl2/SDL.h"
#include "Gameclock.h"

struct gt_Gamestate {
    SDL_bool isLoaded;
    void (*handleEvent)(const SDL_Event *);
    void (*handleKeyState)(const Uint8 *);
    SDL_bool (*load)(void);
    void (*unload)(void);
    void (*update)(void);
    void (*render)(void);
};

struct gt_Gamestate_Machine {
    SDL_bool shouldSkip;
    SDL_bool isPlaying;
    struct gt_Gameclock *clock;
    struct gt_Gamestate **states;
    int next_state_index;
};

/* Game Machine function api */

/**
 * Checks whether the state pointed to, by the next state index, is the end
 * state.
 */
SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *m);

/**
 * Returns the current state and advances the internal index pointer
 */
struct gt_Gamestate *gt_gsmachine_advanceState(struct gt_Gamestate_Machine *m);

/**
 * Initializes the gamestate machine with a gamestate array that specifies the 
 * ordering and available game states.
 */
SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *m, struct gt_Gameclock *c, 
                           struct gt_Gamestate **s);

/**
 * Getter for the current state index.
 */
int gt_gsmachine_getCurrentStateIndex(struct gt_Gamestate_Machine *m);

/**
 * Go to a gamestate by index. Sets the skip flag such that the gamestate runtime 
 * exits the current state and starts on new gamestate.
 */
void gt_gsmachine_goToState(struct gt_Gamestate_Machine *m, int i);

/**
 * Runs the actual gameloop
 */
int gt_gsmachine_runloop(struct gt_Gamestate_Machine *m);


#define GT_STATE_ARRAY_END NULL

#endif