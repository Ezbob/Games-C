
#include "Gamestate.h"

SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *machine) {
    return (machine->states[machine->next_state_index]) != GT_STATE_ARRAY_END;
}

struct gt_Gamestate *gt_gsmachine_advanceState(struct gt_Gamestate_Machine *machine) {
    return machine->states[machine->next_state_index++];
}

SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *machine, struct gt_Gameclock *clock,
                           struct gt_Gamestate **states) {
    machine->isPlaying = SDL_TRUE;
    machine->clock = clock;
    machine->next_state_index = 0;
    machine->states = states;
    return SDL_TRUE;
}

void gt_gsmachine_setCurrentStateIndex(struct gt_Gamestate_Machine *m, int nextState) {
    m->next_state_index = nextState;
}

void gt_gsmachine_goToState(struct gt_Gamestate_Machine *m, int i) {
    m->next_state_index = (i + 1);
    m->shouldSkip = SDL_TRUE;
}

void gt_gstate_pump_events(struct gt_Gamestate *state) {
    static SDL_Event inputEvent;
    while ( SDL_PollEvent(&inputEvent) != 0 ) {
        state->handleEvent(&inputEvent);
    }
    state->handleKeyState(SDL_GetKeyboardState(NULL));
}

struct gt_Gamestate *gt_gsmachine_setupSkip(struct gt_Gamestate_Machine *m) {
    m->shouldSkip = SDL_FALSE;
    return m->states[m->next_state_index - 1];
}

int gt_gsmachine_runloop(struct gt_Gamestate_Machine *machine) {
    struct gt_Gamestate *state = gt_gsmachine_advanceState(machine);

    if ( state == GT_STATE_ARRAY_END ) {
        perror("No valid start game state");
        return 1;
    }

    while ( machine->isPlaying && state != GT_STATE_ARRAY_END ) {

        if ( !state->isLoaded && !(state->isLoaded = state->load()) ) {
            perror("Could not initialize state");
            return 2;
        }

        while ( machine->isPlaying && !machine->shouldSkip ) {

            gt_gstate_pump_events(state);

            while ( GT_CLOCK_SHOULD_UPDATE(machine->clock) ) {
                state->update();

                GT_CLOCK_LAG_UPDATE(machine->clock);
            }

            state->render();

            GT_CLOCK_TICK(machine->clock);
        }

        state = gt_gsmachine_setupSkip(machine);
    }

    return 0;
}
