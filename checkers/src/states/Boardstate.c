#include <stdio.h>
#include "Boardstate.h"
#include "Macros.h"
#include "States.h"

extern SDL_Renderer *g_renderer;
extern struct gt_Gamestate_Machine g_statemachine;

enum board_consts {
    BOARD_LENGTH = 8,
    BOARD_SIZE = (BOARD_LENGTH * BOARD_LENGTH)
};

static const int checkerLength = 60;

enum PlayingColor {
    GREEN,
    RED
};

struct Checker {
    enum PlayingColor color;
    SDL_Rect *rect; // the actual rendered rect
    SDL_Rect next; // the next point in the lerp
};

struct Cell {
    SDL_Rect *container;
    struct Checker *occubant;
    int columnIndex;
    int rowIndex;
};

SDL_bool g_is_target_selected = SDL_FALSE;
SDL_Rect g_board[BOARD_SIZE];
SDL_Rect g_checker_rects[BOARD_SIZE];

struct Score {
    int green_remaining;
    int red_remaining;
    int green_length;
    int red_length;
} g_score;

/* There can be max BOARD_SIZE checkers in play */
struct Checker g_checkers[BOARD_SIZE];
struct Cell g_cellboard[BOARD_SIZE];

struct Cell *g_selected = NULL;
SDL_Point g_mouse;
SDL_Rect g_selectionBox;

enum PlayingColor g_playingColor = GREEN;

void printCell(const struct Cell * c) {
    printf("CELL(%p\t%p)\n", c->container, c->occubant);
}

void printRect(const SDL_Rect * r) {
    printf("RECT(x: %i, y: %i, w: %i, h: %i)\n", r->x, r->y, r->w, r->h);
}

void findSelected() {
    for (int i = 0; i < BOARD_LENGTH; i++) {
        for (int j = 0; j < BOARD_LENGTH; ++j) {

            int index = i * BOARD_LENGTH + j;
            struct Cell *gridCell = g_cellboard + index;

            if ( SDL_PointInRect(&g_mouse, gridCell->container)
                && gridCell->occubant != NULL
                && gridCell->occubant->color == g_playingColor
                && !SDL_RectEmpty(gridCell->occubant->rect)
            ) {
                g_selected = gridCell;
                g_selectionBox.x = g_selected->container->x + 5;
                g_selectionBox.y = g_selected->container->y + 5;

                g_selectionBox.w = g_selected->container->w - 10;
                g_selectionBox.h = g_selected->container->h - 10;
                return;
            }
        }
    }
}

void switchTurn() {
    if (g_playingColor == GREEN) {
        g_playingColor = RED;
    } else {
        g_playingColor = GREEN;
    }
}

void doMoveToEmpty(struct Cell *target) {
    struct Checker *source = g_selected->occubant;

    source->rect->x = target->container->x + 20;
    source->rect->y = target->container->y + 20;

    switchTurn();

    target->occubant = source;
    g_selected->occubant = NULL;
}

void doOvertake(struct Cell *taken, struct Cell *target) {
    struct Checker *source = g_selected->occubant;

    source->rect->x = target->container->x + 20;
    source->rect->y = target->container->y + 20;

    if (taken->occubant->color == GREEN) {
        g_score.green_remaining--;
    } else {
        g_score.red_remaining--;
    }

    target->occubant = source;
    g_selected->occubant = NULL;

    memset(taken->occubant->rect, 0, sizeof(SDL_Rect));

    taken->occubant = NULL;
}

SDL_bool tryToOvertake(struct Cell *clickedGridCell, int xOffset, int yOffset) {
    int y2diff = g_selected->rowIndex + (yOffset * 2);
    int x2diff = g_selected->columnIndex + (xOffset * 2);
    int nextNextIndex = y2diff * BOARD_LENGTH + x2diff;

    if (
        x2diff < 0 ||
        x2diff >= BOARD_LENGTH ||
        y2diff < 0 ||
        y2diff >= BOARD_LENGTH
    ) {
        return SDL_FALSE;
    }

    if ( 0 <= nextNextIndex && nextNextIndex < BOARD_LENGTH * BOARD_LENGTH ) {
        struct Cell *nextNextCell = g_cellboard + nextNextIndex;
        if ( nextNextCell->occubant == NULL) {
            doOvertake(clickedGridCell, nextNextCell);
            return SDL_TRUE;
        }
    }

    return SDL_FALSE;
}

SDL_bool tryToMove(int xOffset, int yOffset) {
    int ydiff = g_selected->rowIndex + yOffset;
    int xdiff = g_selected->columnIndex + xOffset;
    int nextIndex = ydiff * BOARD_LENGTH + xdiff;

    if ( 0 <= nextIndex && nextIndex < (BOARD_LENGTH * BOARD_LENGTH) ) {
        struct Cell *gridCell = g_cellboard + nextIndex;

        if (
            xdiff < 0             ||
            xdiff >= BOARD_LENGTH ||
            ydiff < 0             ||
            ydiff >= BOARD_LENGTH
        ) {
            // constraint against wrap around
            return SDL_FALSE;
        }

        if ( SDL_PointInRect(&g_mouse, gridCell->container) ) {
            if ( gridCell->occubant == NULL ) {
                doMoveToEmpty(gridCell);
                return SDL_TRUE;
            } else if ( gridCell->occubant->color != g_selected->occubant->color ) {
                return tryToOvertake(gridCell, xOffset, yOffset);
            }
        }
    }

    return SDL_FALSE;
}

void updateSelected() {
    if ( tryToMove( 1,  1) ) return;
    if ( tryToMove(-1,  1) ) return;
    if ( tryToMove( 1, -1) ) return;
    if ( tryToMove(-1, -1) ) return;
}

/*
 * External state api
 */

void boardstate_handleEvent(const SDL_Event *event) {
    Uint32 mouseState;
    switch(event->type) {
        case SDL_QUIT:
            gt_gsmachine_goToState(&g_statemachine, GT_END_STATE);
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseState = SDL_GetMouseState(&g_mouse.x, &g_mouse.y);
            if ( mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) ) {
                if (g_selected == NULL) {
                    findSelected();
                } else {
                    g_is_target_selected = SDL_TRUE;
                }
            }
            break;
        default:
            break;
    }
}

void boardstate_handleKeyState(const Uint8 *states) {
    if ( states[SDL_SCANCODE_ESCAPE] ) {
        gt_gsmachine_goToState(&g_statemachine, GT_PAUSE_STATE);
    }
}

void boardstate_unload(void) {}

SDL_bool boardstate_load() {
    for (int i = 0; i < BOARD_LENGTH; ++i) {
        for (int j = 0; j < BOARD_LENGTH; ++j) {

            int flatIndex = (i * BOARD_LENGTH + j);

            SDL_Rect *container = g_board + flatIndex;
            struct Cell *cell = g_cellboard + flatIndex;

            container->h = 100;
            container->w = 100;
            container->x = 100 * (flatIndex % BOARD_LENGTH) + 20;
            container->y = 100 * (flatIndex / BOARD_LENGTH) + 20;

            cell->container = container;
            cell->columnIndex = j;
            cell->rowIndex = i;

            /** GREEN **/
            if ( i % 2 == 0 && i < (BOARD_LENGTH / 2) ) {
                if (j % 2 == 0) {
                    struct Checker *checker = g_checkers + (g_score.green_length);
                    SDL_Rect *rect = g_checker_rects + (g_score.green_length++);

                    rect->x = (100 * (j % BOARD_LENGTH)) + 40;
                    rect->y = (container->y + 20);
                    rect->w = checkerLength;
                    rect->h = checkerLength;

                    checker->color = GREEN;
                    checker->rect = rect;
                    cell->occubant = checker;
                    continue;
                }
            } else if ( i % 2 != 0 && i < (BOARD_LENGTH / 2) - 1) {
                if (j % 2 != 0) {
                    struct Checker *checker = g_checkers + (g_score.green_length);
                    SDL_Rect *rect = g_checker_rects + (g_score.green_length++);

                    rect->x = (100 * (j % BOARD_LENGTH)) + 40;
                    rect->y = (container->y + 20);
                    rect->w = checkerLength;
                    rect->h = checkerLength;

                    checker->color = GREEN;
                    checker->rect = rect;
                    cell->occubant = checker;
                    continue;
                }
            }

            /** RED **/
            if ( i % 2 == 0 && i > (BOARD_LENGTH / 2) ) {
                if (j % 2 == 0) {
                    int currentIndex =  (g_score.green_length + g_score.red_length);
                    struct Checker *checker = g_checkers + currentIndex;
                    SDL_Rect *rect = g_checker_rects + currentIndex;
                    g_score.red_length++;

                    rect->x = (100 * (j % BOARD_LENGTH)) + 40;
                    rect->y = (container->y + 20);
                    rect->w = checkerLength;
                    rect->h = checkerLength;

                    checker->color = RED;
                    checker->rect = rect;
                    cell->occubant = checker;
                    continue;
                }
            } else if ( i % 2 != 0 && i > (BOARD_LENGTH / 2) ) {
                if (j % 2 != 0) {
                    int currentIndex =  (g_score.green_length + g_score.red_length);
                    struct Checker *checker = g_checkers + currentIndex;
                    SDL_Rect *rect = g_checker_rects + currentIndex;
                    g_score.red_length++;

                    rect->x = (100 * (j % BOARD_LENGTH)) + 40;
                    rect->y = (container->y + 20);
                    rect->w = checkerLength;
                    rect->h = checkerLength;

                    checker->color = RED;
                    checker->rect = rect;
                    cell->occubant = checker;
                    continue;
                }
            }
        }
    }

    g_score.green_remaining = g_score.green_length;
    g_score.red_remaining = g_score.red_length;

    return SDL_TRUE;
}

void boardstate_update() {

    if (g_score.red_remaining == 0 || g_score.green_remaining == 0) {
        gt_gsmachine_goToState(&g_statemachine, GT_GAME_OVER_STATE);
        return;
    }

    if (g_selected != NULL && g_is_target_selected == SDL_TRUE) {
        updateSelected();

        memset(&g_selectionBox, 0, sizeof(g_selectionBox));
        g_selected = NULL;
        g_is_target_selected = SDL_FALSE;
    }
}

void boardstate_render() {
    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE);
    SDL_RenderClear(g_renderer);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_BLACK);
    SDL_RenderDrawRects(g_renderer, g_board, BOARD_LENGTH * BOARD_LENGTH);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_GREEN);
    SDL_RenderFillRects(g_renderer, g_checker_rects, g_score.green_length);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_RED);
    SDL_RenderFillRects(g_renderer, g_checker_rects + g_score.green_length, g_score.red_length);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_BLACK);
    SDL_RenderDrawRect(g_renderer, &g_selectionBox);

    SDL_RenderPresent(g_renderer);
}
