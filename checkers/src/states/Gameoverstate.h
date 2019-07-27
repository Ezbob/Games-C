
#ifndef _HEADER_FILE_Gameoverstate_20190725190201_
#define _HEADER_FILE_Gameoverstate_20190725190201_

#include "SDL.h"

SDL_bool gameoverstate_load(void);
void gameoverstate_render(void);
void gameoverstate_update(void);
void gameoverstate_handleKeyState(const Uint8 *ks);
void gameoverstate_handleEvent(const SDL_Event *e);
void gameoverstate_unload(void);

#endif
