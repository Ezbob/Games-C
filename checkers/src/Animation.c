
#include <stdlib.h>
#include "Animation.h"

void gt_animation_init(struct gt_animation *a, Uint64 dur) {
    a->startTick = 0;
    a->isRunning = SDL_FALSE;
    a->duration = dur;
    a->atEnd = NULL;
    a->atUpdate = NULL;
    a->updateData = NULL;
    a->endData = NULL;
}

void gt_animation_start(struct gt_animation *a) {
    a->startTick = SDL_GetTicks();
    a->isRunning = SDL_TRUE;
}

void gt_animation_stop(struct gt_animation *a) {
    a->isRunning = SDL_FALSE;
}

void gt_animation_register_at_update(struct gt_animation *a, void (*atUpdate)(double, void *),
                                     void *updateData) {
    a->atUpdate = atUpdate;
    a->updateData = updateData;
}

void gt_animation_register_at_end(struct gt_animation *a, void (*atEnd)(void *), void *endData) {
    a->atEnd = atEnd;
    a->endData = endData;
}

void gt_animation_tick(struct gt_animation *a, Uint64 ct) {
    if ( a->isRunning ) {
        a->isRunning = (ct <= (a->startTick + a->duration));
        if ( a->atUpdate != NULL )
            a->atUpdate(((double) (ct - a->startTick) / a->duration), a->updateData);
        if ( !a->isRunning && a->atEnd != NULL )
            a->atEnd(a->endData);
    }
}
