
#include "Gamestate.h"

SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *machine, struct gt_Gameclock *clock,
                           struct gt_Gamestate **states, int nstates) {
    machine->clock = clock;
    machine->currentState = 0;
    machine->states = states;
    machine->shouldRun = SDL_TRUE;
    machine->numberOfState = nstates;
    return SDL_TRUE;
}

void gt_gsmachine_goToState(struct gt_Gamestate_Machine *m, int i) {
    m->currentState = i;
    m->shouldRun = SDL_FALSE;
}

void gt_gsmachine_unloadAll(struct gt_Gamestate_Machine *machine) {
    for (int i = 0; i < (machine->numberOfState - 1); ++i) {
        struct gt_Gamestate *state = machine->states[i];
        if (state->isLoaded) state->unload();
    }
}

int gt_gsmachine_runLoop(struct gt_Gamestate_Machine *machine) {
    struct gt_Gamestate *state = machine->states[machine->currentState];
    SDL_Event inputEvent;

    if ( state == GT_STATE_ARRAY_END ) {
        perror("No valid start game state");
        return 1;
    }

    while ( state != GT_STATE_ARRAY_END ) {

        if ( !state->isLoaded && !(state->isLoaded = state->load()) ) {
            perror("Could not initialize state");
            return 2;
        }

        while ( machine->shouldRun ) {

            /* input polling */
            while ( SDL_PollEvent(&inputEvent) != 0 ) {
                state->handleEvent(&inputEvent);
            }
            state->handleKeyState(SDL_GetKeyboardState(NULL));

            /* desynced update loop */
            while ( GT_CLOCK_SHOULD_UPDATE(machine->clock) ) {
                state->update();

                GT_CLOCK_LAG_UPDATE(machine->clock);
            }

            /* render phase */
            state->render();

            GT_CLOCK_TICK(machine->clock);
        }

        machine->shouldRun = SDL_TRUE;
        state = machine->states[machine->currentState];
    }

    return 0;
}
