
#include "Gamestate.h"

SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *machine) {
    return (machine->states[(machine->current_state_index + 1)]) != &GT_ENDSTATE;
}

struct gt_Gamestate *gt_gsmachine_advanceState(struct gt_Gamestate_Machine *machine) {
    if (!machine)
        return NULL;
    return machine->states[machine->current_state_index++];
}

SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *machine, struct gt_Gamestate **states) {
    machine->current_state_index = 0;
    machine->states = states;
    return SDL_TRUE;
}

void gt_gstate_pump_events(struct gt_Gamestate *state) {
    static SDL_Event inputEvent;
    while ( SDL_PollEvent(&inputEvent) != 0 ) {
        state->handleEvent(&inputEvent);
    }
    state->handleKeyState(SDL_GetKeyboardState(NULL));
}
