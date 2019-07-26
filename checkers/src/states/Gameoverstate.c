
#include "Gameoverstate.h"
#include "sdl2/SDL.h"
#include "sdl2/SDL_ttf.h"
#include "Convert.h"
#include "Shared.h"
#include "Macros.h"
#include "States.h"

extern SDL_Renderer *g_renderer;
extern struct gt_Gamestate_Machine g_statemachine;
extern int g_playingColor;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

TTF_Font *font;
SDL_Texture *texture[2];
SDL_Rect position[2];

SDL_bool gameoverstate_load(void) {
    font = TTF_OpenFont("assets/consola.ttf", 32);

    if (!font) {
        perror(TTF_GetError());
        return SDL_FALSE;
    }

    texture[0] = GT_LOAD_BLACK_TEXT("Gameover", font);
    if (!texture[0])
        goto sdl_error_state;

    SDL_QueryTexture(texture[0], NULL, NULL, &position[0].w, &position[0].h);
    position[0].x = SCREEN_WIDTH / 2 - (position[0].w / 2);
    position[0].y = SCREEN_HEIGHT / 2;

    SDL_Texture *text;
    if (g_playingColor == 0) {
        text = convert_to_texture(
            g_renderer,
            TTF_RenderText_Solid(font, "Green won!", (SDL_Color){PC_OPAQUE_GREEN})
        );
    } else {
        text = convert_to_texture(
            g_renderer, 
            TTF_RenderText_Solid(font, "Red won!", (SDL_Color){PC_OPAQUE_RED})
        );
    }

    if (!text)
        goto sdl_error_state;

    texture[1] = text;

    SDL_QueryTexture(text, NULL, NULL, &position[1].w, &position[1].h);
    position[1].x = SCREEN_WIDTH / 2 - (position[1].w / 2);
    position[1].y = SCREEN_HEIGHT / 2 + (position[1].h + 4);

    return SDL_TRUE;

sdl_error_state:
    perror(SDL_GetError());
    return SDL_FALSE;
}

void gameoverstate_unload(void) {
    SDL_DestroyTexture(texture[0]);
    SDL_DestroyTexture(texture[1]);
}

void gameoverstate_render(void) {
    SDL_SetRenderDrawColor(g_renderer, PC_OPAQUE_WHITE);
    SDL_RenderClear(g_renderer);

    SDL_RenderCopy(g_renderer, texture[0], NULL, &position[0]);
    SDL_RenderCopy(g_renderer, texture[1], NULL, &position[1]);
    SDL_RenderPresent(g_renderer);
}

void gameoverstate_update(void) {

}

void gameoverstate_handleKeyState(const Uint8 *ks) {
    if ( ks[SDL_SCANCODE_ESCAPE] ) {
        gt_gsmachine_goToState(&g_statemachine, GT_END_STATE);
    }
}

void gameoverstate_handleEvent(const SDL_Event *e) {
    switch (e->type) {
        case SDL_QUIT:
            gt_gsmachine_goToState(&g_statemachine, GT_END_STATE);
            break;
        default:
            break;
    }
}


