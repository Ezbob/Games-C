
#include <stdlib.h>
#include "Animation.h"

void gt_animation_init(struct gt_animation *a, Uint64 dur, void (*atUpdate)(double, void *),
    void *data) {
    a->startTick = 0;
    a->isRunning = SDL_FALSE;
    a->duration = dur;
    a->atEnd = NULL;
    a->atUpdate = atUpdate;
    a->updateData = data;
    a->endData = NULL;
}

void gt_animation_start(struct gt_animation *a) {
    a->startTick = SDL_GetTicks();
    a->isRunning = SDL_TRUE;
}

void gt_animation_stop(struct gt_animation *a) {
    a->isRunning = SDL_FALSE;
}

void gt_animation_register_at_end(struct gt_animation *a, void (*atEnd)(void *), void *endData) {
    a->atEnd = atEnd;
    a->endData = endData;
}

void gt_animation_tick(struct gt_animation *a, Uint64 ct) {
    if ( a->isRunning ) {
        a->isRunning = (ct <= (a->startTick + a->duration));
        a->atUpdate(((double) (ct - a->startTick) / a->duration), a->updateData);
        if (!a->isRunning && a->atEnd != NULL)
            a->atEnd(a->endData);
    }
}
