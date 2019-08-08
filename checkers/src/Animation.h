
#ifndef _HEADER_FILE_Animation_20190805212234_
#define _HEADER_FILE_Animation_20190805212234_

#include "SDL.h"

struct gt_animation {
    Uint64 startTick;
    SDL_bool isRunning;
    Uint64 duration;
    void *updateData;
    void *endData;
    void (*atEnd)(void *);
    void (*atUpdate)(double, void *);
};

void gt_animation_init(struct gt_animation *a, Uint64 dur, void (*atUpdate)(double, void *), void *data);
void gt_animation_start(struct gt_animation *a);
void gt_animation_stop(struct gt_animation *a);
void gt_animation_tick(struct gt_animation *a, Uint64 ct);
void gt_animation_register_at_end(struct gt_animation *a, void (*atEnd)(void *), void *endData);

#define GT_ANIMATION_IS_ENDED(a, ct) (ct > (a.startTick + a.duration))
#define GT_ANIMATION_CALC_REMAIN(currentTicks, a) ((double) (currentTicks - a.startTick) / a.duration)

#endif