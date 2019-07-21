
#include "SDL.h"
#include "Gameclock.h"
#include <string.h>

void gt_gameclock_init(struct gt_Gameclock *clock, double msPerUpdate) {
    memset(clock, 0, sizeof(struct gt_Gameclock));
    clock->now = SDL_GetPerformanceCounter();
    clock->msPerUpdate = msPerUpdate;
}

void gt_gameclock_tick(struct gt_Gameclock *clock) {
    clock->last = clock->now;
    clock->now = SDL_GetPerformanceCounter();
    clock->frameElapsed = ((clock->now - clock->last) * 1000) / (double)(SDL_GetPerformanceFrequency());
    clock->updateLag += clock->frameElapsed;
}
