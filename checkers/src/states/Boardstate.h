
#ifndef _HEADER_FILE_Boardstate_20190721231022_
#define _HEADER_FILE_Boardstate_20190721231022_

#include "sdl2/SDL.h"

SDL_bool boardstate_load(void);
void boardstate_render(void);
void boardstate_update(void);
void boardstate_handleKeyState(const Uint8 *ks);
void boardstate_handleEvent(const SDL_Event *e);
void boardstate_unload(void);

#endif