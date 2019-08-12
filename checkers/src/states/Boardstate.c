#include <stdio.h>
#include "Boardstate.h"
#include "Macros.h"
#include "States.h"
#include "Tweening.h"
#include "Animation.h"


#define IS_AXIS_WITHIN_BOARD(x) (0 <= x && x < BOARD_LENGTH)
#define IS_IN_BOUNDS(x, y) ((0 <= x && x < BOARD_LENGTH) && (0 <= y && y < BOARD_LENGTH))
#define BOARD_INDEX(x, y) (y * BOARD_LENGTH + x)

extern SDL_Renderer *g_renderer;
extern struct gt_Gamestate_Machine g_statemachine;

enum board_consts {
    BOARD_LENGTH = 8,
    BOARD_SIZE = (BOARD_LENGTH * BOARD_LENGTH)
};

static const int checkerLength = 60;
static const int containerLength = 100;

enum PlayingColor {
    GREEN,
    RED
};

struct Checker {
    enum PlayingColor color;
    struct gt_animation anim;
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

void showSelectionBox() {
    g_selectionBox.x = g_selected->container->x + 5;
    g_selectionBox.y = g_selected->container->y + 5;

    g_selectionBox.w = g_selected->container->w - 10;
    g_selectionBox.h = g_selected->container->h - 10;
}

void findSelected() {
    for (int i = 0; i < BOARD_LENGTH; i++) {
        for (int j = 0; j < BOARD_LENGTH; ++j) {
            struct Cell *gridCell = g_cellboard + BOARD_INDEX(j, i);

            if ( SDL_PointInRect(&g_mouse, gridCell->container)
                && gridCell->occubant != NULL
                && gridCell->occubant->color == g_playingColor
                && !SDL_RectEmpty(gridCell->occubant->rect)
            ) {
                g_selected = gridCell;

                return showSelectionBox();
            }
        }
    }
}

void moveChecker(struct Checker *checker, int x, int y) {
    checker->next.x = x;
    checker->next.y = y;
    gt_animation_start(&checker->anim);
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

    moveChecker(source, target->container->x + 20, target->container->y + 20);

    switchTurn();

    target->occubant = source;
    g_selected->occubant = NULL;
}

void doOvertake(struct Cell *taken, struct Cell *target) {
    struct Checker *source = g_selected->occubant;

    moveChecker(source, target->container->x + 20, target->container->y + 20);

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

    if ( !IS_IN_BOUNDS(x2diff, y2diff) ) {
        return SDL_FALSE;
    }

    struct Cell *nextNextCell = g_cellboard + BOARD_INDEX(x2diff, y2diff);
    if ( nextNextCell->occubant == NULL) {
        doOvertake(clickedGridCell, nextNextCell);
        return SDL_TRUE;
    }

    return SDL_FALSE;
}

SDL_bool tryToMove(int xOffset, int yOffset) {
    int ydiff = g_selected->rowIndex + yOffset;
    int xdiff = g_selected->columnIndex + xOffset;

    if ( !IS_IN_BOUNDS(xdiff, ydiff) )
        return SDL_FALSE;

    struct Cell *gridCell = g_cellboard + BOARD_INDEX(xdiff, ydiff);

    if ( SDL_PointInRect(&g_mouse, gridCell->container) ) {
        if ( gridCell->occubant == NULL ) {
            doMoveToEmpty(gridCell);
            return SDL_TRUE;
        } else if ( gridCell->occubant->color != g_selected->occubant->color ) {
            return tryToOvertake(gridCell, xOffset, yOffset);
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

void checkerAnimationUpdate(double remaining, void *data) {
    struct Checker *checker = (struct Checker *) data;
    lerp(checker->rect, checker->rect, &checker->next, remaining);
}

void initCheckerPosition(struct Checker *checker, SDL_Rect *rect,
    int x, int y, int w, int h, enum PlayingColor color) {
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;

    /* Next tweening position is
        initially the same as the
        start */
    checker->next = *rect;
    checker->color = color;
    checker->rect = rect;

    gt_animation_init(&checker->anim, 1500);
    gt_animation_register_at_update(&checker->anim, checkerAnimationUpdate, (void *) checker);
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

            container->h = containerLength;
            container->w = containerLength;
            container->x = containerLength * (flatIndex % BOARD_LENGTH) + 20;
            container->y = containerLength * (flatIndex / BOARD_LENGTH) + 20;

            cell->container = container;
            cell->columnIndex = j;
            cell->rowIndex = i;

            /** Checkers initialization **/

            /** GREEN **/
            if ( i % 2 == 0 && i < (BOARD_LENGTH / 2) ) {
                if (j % 2 == 0) {
                    struct Checker *checker = g_checkers + (g_score.green_length);
                    SDL_Rect *rect = g_checker_rects + (g_score.green_length++);

                    initCheckerPosition(checker, rect,
                        (containerLength * (j % BOARD_LENGTH)) + 40,
                        (container->y + 20),
                        checkerLength,
                        checkerLength,
                        GREEN
                    );

                    cell->occubant = checker;
                    continue;
                }
            } else if ( i % 2 != 0 && i < (BOARD_LENGTH / 2) - 1 ) {
                if (j % 2 != 0) {
                    struct Checker *checker = g_checkers + (g_score.green_length);
                    SDL_Rect *rect = g_checker_rects + (g_score.green_length++);

                    initCheckerPosition(checker, rect,
                        containerLength * (j % BOARD_LENGTH) + 40,
                        container->y + 20,
                        checkerLength,
                        checkerLength,
                        GREEN
                    );

                    cell->occubant = checker;
                    continue;
                }
            }

            /** RED **/
            if ( i % 2 == 0 && i > (BOARD_LENGTH / 2) ) {
                if (j % 2 == 0) {
                    int currentIndex = (g_score.green_length + g_score.red_length);
                    struct Checker *checker = g_checkers + currentIndex;
                    SDL_Rect *rect = g_checker_rects + currentIndex;
                    g_score.red_length++;

                    initCheckerPosition(checker, rect,
                        containerLength * (j % BOARD_LENGTH) + 40,
                        container->y + 20,
                        checkerLength,
                        checkerLength,
                        RED
                    );

                    cell->occubant = checker;
                    continue;
                }
            } else if ( i % 2 != 0 && i > (BOARD_LENGTH / 2) ) {
                if (j % 2 != 0) {
                    int currentIndex =  (g_score.green_length + g_score.red_length);
                    struct Checker *checker = g_checkers + currentIndex;
                    SDL_Rect *rect = g_checker_rects + currentIndex;
                    g_score.red_length++;

                    initCheckerPosition(checker, rect,
                        containerLength * (j % BOARD_LENGTH) + 40,
                        container->y + 20,
                        checkerLength,
                        checkerLength,
                        RED
                    );

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
    Uint32 ticks = SDL_GetTicks();
    if ( g_score.red_remaining == 0 || g_score.green_remaining == 0 ) {
        gt_gsmachine_goToState(&g_statemachine, GT_GAME_OVER_STATE);
        return;
    }

    if ( g_selected != NULL && g_is_target_selected == SDL_TRUE ) {
        updateSelected();

        memset(&g_selectionBox, 0, sizeof(g_selectionBox));
        g_selected = NULL;
        g_is_target_selected = SDL_FALSE;
    }

    for ( int i = 0; i < (g_score.green_length + g_score.red_length); i++ ) {
        struct Checker *c = g_checkers + i;
        gt_animation_tick(&c->anim, ticks);
    }
}

void renderCheckerTracers(int nextRow, int nextColumn, int next2Row, int next2Column) {
    if ( IS_IN_BOUNDS(nextRow, nextColumn) ) {
        struct Cell *gridCell = g_cellboard + BOARD_INDEX(nextColumn, nextRow);

        if ( gridCell->occubant == NULL ) {
            SDL_SetRenderDrawColor(g_renderer, 0x00, 0x7f, 0xff, 0xff);
        } else if ( gridCell->occubant->color != g_selected->occubant->color
                    && IS_IN_BOUNDS(next2Row, next2Column) ) {
            struct Cell *nextGridCell = g_cellboard + BOARD_INDEX(next2Column, next2Row);

            if ( nextGridCell->occubant == NULL ) {
                SDL_SetRenderDrawColor(g_renderer, 0x00, 0x7f, 0xff, 0xff);
            } else {
                SDL_SetRenderDrawColor(g_renderer, 0x54, 0x54,0x54, 0xff);
            }
        } else {
            SDL_SetRenderDrawColor(g_renderer, 0x54, 0x54,0x54, 0xff);
        }
        SDL_RenderFillRect(g_renderer, gridCell->container);
    }
}

void boardstate_render() {
    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE);
    SDL_RenderClear(g_renderer);

    if ( g_selected != NULL ) {
        int nextRow = g_selected->rowIndex + 1;
        int nextColumn = g_selected->columnIndex + 1;
        int prevRow = g_selected->rowIndex - 1;
        int prevColumn = g_selected->columnIndex - 1;

        int next2Row = g_selected->rowIndex + 2;
        int next2Column = g_selected->columnIndex + 2;
        int prev2Row = g_selected->rowIndex - 2;
        int prev2Column = g_selected->columnIndex - 2;

        renderCheckerTracers(nextRow, nextColumn, next2Row, next2Column);
        renderCheckerTracers(nextRow, prevColumn, next2Row, prev2Column);
        renderCheckerTracers(prevRow, nextColumn, prev2Row, next2Column);
        renderCheckerTracers(prevRow, prevColumn, prev2Row, prev2Column);
    }

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
