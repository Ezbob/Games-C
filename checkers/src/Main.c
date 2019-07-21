#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "Gameclock.h"
#include "Gamestate.h"

#if defined(_WIN32)
    #define MAIN_NAME WinMain
#else
    #define MAIN_NAME main
#endif

#define UNUSED(x) ((void) x)

// PC = Parameter Color
//           name        r    g    b    a
#define PC_OPAQUE_RED   0xff,0x00,0x00,0xff
#define PC_OPAQUE_GREEN 0x00,0xff,0x00,0xff
#define PC_OPAQUE_BLUE  0x00,0x00,0xff,0xff
#define PC_OPAQUE_WHITE 0xff,0xff,0xff,0xff
#define PC_OPAQUE_BLACK 0x00,0x00,0x00,0xff

enum board_consts {
    BOARD_LENGTH = 8,
    BOARD_SIZE = (BOARD_LENGTH * BOARD_LENGTH)
};

static const double MS_PER_UPDATE = 16.0;
static const int SCREEN_WIDTH = 840;
static const int SCREEN_HEIGHT = 860;

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

static const char *g_window_title = "Pure C Checkers";
SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_bool g_is_playing = SDL_TRUE;
struct gt_Gameclock g_gameclock;
struct gt_Gamestate_Machine g_statemachine;

SDL_bool g_target = SDL_FALSE;
SDL_Rect g_board[BOARD_LENGTH * BOARD_LENGTH];
SDL_Rect g_checker_rects[BOARD_LENGTH * BOARD_LENGTH];

int g_green_length = 0;
int g_red_length = 0;

struct Checker g_checkers[BOARD_LENGTH * BOARD_LENGTH];
struct Cell g_cellboard[BOARD_LENGTH * BOARD_LENGTH];

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

SDL_bool sdlInit() {
    if ( SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT,
                                     SDL_WINDOW_SHOWN, &g_window, &g_renderer) == -1 )
        goto error_label;

    if ( SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE) == -1 )
        goto error_label;

    if ( SDL_RenderClear(g_renderer) == -1 )
        goto error_label;

    SDL_SetWindowTitle(g_window, g_window_title);

    return SDL_TRUE;

error_label:
    perror(SDL_GetError());
    return SDL_FALSE;
}

void sdlDestroy() {
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
}

void handleEvent(const SDL_Event *event) {
    Uint32 mouseState;
    switch(event->type) {
        case SDL_QUIT:
            g_is_playing = SDL_FALSE;
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseState = SDL_GetMouseState(&g_mouse.x, &g_mouse.y);
            if ( mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) ) {
                if (g_selected == NULL) {
                    findSelected();
                } else {
                    g_target = SDL_TRUE;
                }
            }
            break;
        default:
            break;
    }
}

void handleKeyState(const Uint8 *states) {
    if (states[SDL_SCANCODE_ESCAPE])
        g_is_playing = SDL_FALSE;
}

SDL_bool load() {
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
                    struct Checker *checker = g_checkers + (g_green_length);
                    SDL_Rect *rect = g_checker_rects + (g_green_length++);

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
                    struct Checker *checker = g_checkers + (g_green_length);
                    SDL_Rect *rect = g_checker_rects + (g_green_length++);

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
                    int currentIndex =  (g_green_length + g_red_length);
                    struct Checker *checker = g_checkers + currentIndex;
                    SDL_Rect *rect = g_checker_rects + currentIndex;
                    g_red_length++;

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
                    int currentIndex =  (g_green_length + g_red_length);
                    struct Checker *checker = g_checkers + currentIndex;
                    SDL_Rect *rect = g_checker_rects + currentIndex;
                    g_red_length++;

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

    return SDL_TRUE;
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

void update() {
    if (g_selected != NULL && g_target == SDL_TRUE) {
        updateSelected();

        memset(&g_selectionBox, 0, sizeof(g_selectionBox));
        g_selected = NULL;
        g_target = SDL_FALSE;
    }
}

void render() {
    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE);
    SDL_RenderClear(g_renderer);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_BLACK);
    SDL_RenderDrawRects(g_renderer, g_board, BOARD_LENGTH * BOARD_LENGTH);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_GREEN);
    SDL_RenderFillRects(g_renderer, g_checker_rects, g_green_length);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_RED);
    SDL_RenderFillRects(g_renderer, g_checker_rects + g_green_length, g_red_length);

    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_BLACK);
    SDL_RenderDrawRect(g_renderer, &g_selectionBox);

    SDL_RenderPresent(g_renderer);
}

void pumpEvents() {
    static SDL_Event inputEvent;
    while ( SDL_PollEvent(&inputEvent) != 0 ) {
        handleEvent(&inputEvent);
    }
    handleKeyState(SDL_GetKeyboardState(NULL));
}

void initCells(struct Cell *cs, int length) {
    for (int i = 0; i < length; ++i) {
        cs->container = NULL;
        cs->occubant = NULL;
    }
}

void initGlobalData() {
    initCells(g_cellboard, BOARD_LENGTH * BOARD_LENGTH);
    gt_gameclock_init(&g_gameclock, MS_PER_UPDATE);
    gt_gsmachine_initialize(&g_statemachine);
}

int MAIN_NAME(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    if ( !sdlInit() ) {
        return EXIT_FAILURE;
    }

    initGlobalData();

    if ( !load() ) {
        return EXIT_FAILURE;
    }

    while ( g_is_playing ) {
        pumpEvents();

        while ( GT_CLOCK_SHOULD_UPDATE(g_gameclock) ) {
            update();

            GT_CLOCK_LAG_UPDATE(g_gameclock);
        }

        render();

        GT_CLOCK_TICK(g_gameclock);
    }

    sdlDestroy();
    return EXIT_SUCCESS;
}
