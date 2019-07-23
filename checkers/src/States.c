
#include "States.h"
#include "Gamestate.h"

extern struct gt_Gamestate gs_boardstate;
extern struct gt_Gamestate gs_pausestate;

struct gt_Gamestate *gt_gamestates[GT_NUMBER_OF_STATES] = {
    &gs_boardstate,
    &gs_pausestate,
    GT_STATE_ARRAY_END
};
