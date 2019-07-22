
#ifndef _HEADER_FILE_Pausestate_20190722214505_
#define _HEADER_FILE_Pausestate_20190722214505_

#include "sdl2/SDL.h"

SDL_bool pausestate_load(void);
void pausestate_render(void);
void pausestate_update(void);
void pausestate_handleKeyState(const Uint8 *ks);
void pausestate_handleEvent(const SDL_Event *e);

#endif