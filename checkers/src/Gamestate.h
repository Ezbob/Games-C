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

/**
 * This needs to be at the end of the state array
 * or else you don't have any exit state
 */
#define GT_STATE_ARRAY_END NULL

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
    SDL_bool shouldRun;
    struct gt_Gameclock *clock;
    struct gt_Gamestate **states;
    int numberOfState;
    int currentState;
};

/* Game Machine function api */

/**
 * Initializes the gamestate machine with a gamestate array that specifies the
 * ordering and available game states.
 */
SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *m, struct gt_Gameclock *c,
                           struct gt_Gamestate **s, int nstates);

/**
 * Go to a gamestate by index. Sets the skip flag such that the gamestate runtime
 * exits the current state and starts on new gamestate.
 */
void gt_gsmachine_goToState(struct gt_Gamestate_Machine *m, int i);

/**
 * Runs the actual gameloop
 */
int gt_gsmachine_runLoop(struct gt_Gamestate_Machine *m);

/**
 * Runs the unload function for all states
 */
void gt_gsmachine_unloadAll(struct gt_Gamestate_Machine *m);

#endif