#ifndef __DRGN_FONT_H__
#define __DRGN_FONT_H__

#include "gfc_text.h"
#include "gfc_color.h"
#include "gf2d_graphics.h"

typedef enum
{
	DRGN_SMALL_FONT,
	DRGN_MEDIUM_FONT,
	DRGN_LARGE_FONT,
	DRGN_MAX_FONT
}
DRGN_FontStyles;

typedef struct
{
	SDL_Texture* texture; //texture of the cached font
	Vector2D size; //size of the cached font
	char* text; //text of the cached font
	DRGN_FontStyles style; //style of the cached font
	Color color; //color of the cached font
	Uint32 timestamp; //timestamp the cached font was created
}
DRGN_FontCache;

void drgn_fontInit();

void drgn_fontFree();

/*
* @brief draws text to the screen
* @param text the text to be drawn
* @param style the font style to be used
* @param color the color of the text
* @param pos the position of the text in world space
*/
void drgn_fontDraw(const char* text, DRGN_FontStyles style, Color color, Vector2D pos);

DRGN_FontCache* drgn_fontGetCached(const char* text, DRGN_FontStyles style, Color color);

void drgn_fontAddCached(const char* text, DRGN_FontStyles style, Color color, SDL_Texture* texture, Vector2D size);

void drgn_fontCacheListFree();

void drgn_fontCacheFree(DRGN_FontCache* cache);
#endif
