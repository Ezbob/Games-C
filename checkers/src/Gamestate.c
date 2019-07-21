
#include "Gamestate.h"


SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *machine) {
    return (machine->states[(machine->current_state_index + 1)]) != &GT_ENDSTATE;
}

struct gt_Gamestate *gt_gsmachine_advanceState(struct gt_Gamestate_Machine *machine) {
    if (!machine)
        return NULL;
    return machine->states[machine->current_state_index++];
}

SDL_bool gt_gsmachine_init(struct gt_Gamestate_Machine *machine) {
    machine->current_state_index = 0;
    machine->states = GT_GAMESTATES;
    return SDL_TRUE;
}
