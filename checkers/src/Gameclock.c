
#include "SDL.h"
#include "Gameclock.h"
#include <string.h>

void GT_gameclock_init(struct GameClock *clock, double msPerUpdate) {
    memset(clock, 0, sizeof(struct GameClock));
    clock->now = SDL_GetPerformanceCounter();
    clock->msPerUpdate = msPerUpdate;
}

void GT_gameclock_tick(struct GameClock *clock) {
    clock->last = clock->now;
    clock->now = SDL_GetPerformanceCounter();
    clock->frameElapsed = ((clock->now - clock->last) * 1000) / (double)(SDL_GetPerformanceFrequency());
    clock->updateLag += clock->frameElapsed;
}
