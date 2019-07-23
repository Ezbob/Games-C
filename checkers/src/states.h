
#ifndef _HEADER_FILE_states_20190723194708_
#define _HEADER_FILE_states_20190723194708_

#include "Gamestate.h"

enum gt_states {
    GT_BOARD_STATE = 0,
    GT_PAUSE_STATE,
    GT_END_STATE,
    GT_NUMBER_OF_STATES
};

#define GT_SETUP_GAMESTATE(prefix) { \
    .load = prefix## _load, \
    .update = prefix## _update, \
    .render = prefix## _render, \
    .handleEvent = prefix## _handleEvent, \
    .handleKeyState = prefix## _handleKeyState \
}

#endif