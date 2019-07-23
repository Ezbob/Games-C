
#include "States.h"
#include "Gamestate.h"
#include "states/Boardstate.h"
#include "states/Pausestate.h"

struct gt_Gamestate gs_boardstate = GT_SETUP_GAMESTATE(boardstate);
struct gt_Gamestate gs_pausestate = GT_SETUP_GAMESTATE(pausestate);

struct gt_Gamestate *gt_gamestates[GT_NUMBER_OF_STATES] = {
    &gs_boardstate,
    &gs_pausestate,
    GT_STATE_ARRAY_END
};
