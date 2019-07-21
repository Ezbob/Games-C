
#include "Gamestate.h"


SDL_bool gt_gsmachine_hasNextState(struct gt_Gamestate_Machine *machine) {
    return (machine->states[(machine->current_state_index + 1)]) != &GT_ENDSTATE;
}

struct gt_Gamestate *gt_gsmachine_getNextState(struct gt_Gamestate_Machine *machine) {
    if (!machine)
        return NULL;
    return gt_gsmachine_hasNextState(machine) ? (machine->states + (++machine->current_state_index)) : NULL;
}


SDL_bool gt_gsmachine_initialize(struct gt_Gamestate_Machine *machine) {
    machine->current_state_index = 0;
    machine->states = &GT_GAMESTATES;
    return SDL_TRUE;
}
