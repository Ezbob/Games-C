
#ifndef _HEADER_FILE_Macros_20190721231614_
#define _HEADER_FILE_Macros_20190721231614_

// PC = Parameter Color
//           name        r    g    b    a
#define PC_OPAQUE_RED   0xff,0x00,0x00,0xff
#define PC_OPAQUE_GREEN 0x00,0xff,0x00,0xff
#define PC_OPAQUE_BLUE  0x00,0x00,0xff,0xff
#define PC_OPAQUE_WHITE 0xff,0xff,0xff,0xff
#define PC_OPAQUE_BLACK 0x00,0x00,0x00,0xff

#define UNUSED(x) ((void) x)

#define GT_LOAD_BLACK_TEXT(text, font) convert_from_texture(g_renderer, TTF_RenderText_Solid(font, text, (SDL_Color) {PC_OPAQUE_BLACK}) )

#endif