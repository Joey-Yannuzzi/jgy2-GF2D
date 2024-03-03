#include <SDL_ttf.h>
#include "simple_logger.h"
#include "gfc_list.h"
#include "drgn_font.h"

typedef struct
{
	List* fonts; //list of available fonts
	List* cache; //list of cached rendered texts
}
DRGN_FontManager;

static DRGN_FontManager _fontManager = { 0 };

void drgn_fontFree()
{
	int count;
	TTF_Font* font;
	DRGN_FontCache* cache;

	count = gfc_list_get_count(_fontManager.fonts);

	for (int bogus = 0; bogus < count; bogus++)
	{
		font = (TTF_Font*)gfc_list_get_nth(_fontManager.fonts, bogus);

		if (!font)
		{
			continue;
		}

		TTF_CloseFont(font);
	}

	gfc_list_delete(_fontManager.fonts);
	count = gfc_list_get_count(_fontManager.cache);

	for (int bogus = 0; bogus < count; bogus++)
	{
		cache = (DRGN_FontCache*)gfc_list_get_nth(_fontManager.cache, bogus);

		if (!cache)
		{
			continue;
		}

		drgn_fontCacheFree(cache);
	}

	gfc_list_delete(_fontManager.cache);
	memset(&_fontManager, 0, sizeof(DRGN_FontManager));
	TTF_Quit();
}

void drgn_fontInit()
{
	TTF_Font* font;
	if (TTF_Init() == -1)
	{
		slog("Error initalizing font system");
		return;
	}

	_fontManager.fonts = gfc_list_new();
	_fontManager.cache = gfc_list_new();

	if (!_fontManager.fonts || !_fontManager.cache)
	{
		slog("Could not initialize list");
		return;
	}

	for (int bogus = 0; bogus < DRGN_MAX_FONT; bogus++)
	{
		font = TTF_OpenFont("images/fonts/IMMORTAL.ttf", 20 + (bogus * 4));

		if (!font)
		{
			slog("could not find font");
			continue;
		}

		gfc_list_append(_fontManager.fonts, font);
	}

	atexit(drgn_fontFree);
}

void drgn_fontDraw(const char* text, DRGN_FontStyles style, Color color, Vector2D pos)
{
	TTF_Font* font;
	SDL_Surface* result;
	SDL_Color foregroundColor;
	SDL_Texture* texture;
	SDL_Rect destination;
	DRGN_FontCache* cache;

	cache = drgn_fontGetCached(text, style, color);

	if (cache)
	{
		destination.x = pos.x;
		destination.y = pos.y;
		destination.w = cache->size.x;
		destination.h = cache->size.y;
		SDL_RenderCopy(gf2d_graphics_get_renderer(), cache->texture, NULL, &destination);
		cache->timestamp = SDL_GetTicks();
		slog("drawing text '%s' from cache", text);
		return;
	}

	slog("drawing text '%s' fresh", text);

	font = (TTF_Font*)gfc_list_get_nth(_fontManager.fonts, style);

	if (!font)
	{
		slog("could not find font with style %i", style);
		return;
	}

	foregroundColor = gfc_color_to_sdl(color);

	result = TTF_RenderUTF8_Blended_Wrapped(font, text, foregroundColor, 0);

	if (!result)
	{
		slog("Failed to render text '%s'", text);
		return;
	}

	result = gf2d_graphics_screen_convert(&result);

	if (!result)
	{
		slog("Failed to convert text '%s' properly", text);
		return;
	}

	texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(), result);

	if (!texture)
	{
		slog("Failed to create texture from surface for text '%s'", text);
		SDL_FreeSurface(result);
		return;
	}

	destination.x = pos.x;
	destination.y = pos.y;
	destination.w = result->w;
	destination.h = result->h;
	SDL_RenderCopy(gf2d_graphics_get_renderer(), texture, NULL, &destination);
	SDL_FreeSurface(result);
	drgn_fontAddCached(text, style, color, texture, vector2d(destination.w, destination.h));
}

DRGN_FontCache* drgn_fontGetCached(const char* text, DRGN_FontStyles style, Color color)
{
	DRGN_FontCache* cache;
	int count;

	if (!text)
	{
		slog("No text given");
		return NULL;
	}

	count = gfc_list_get_count(_fontManager.cache);

	for (int bogus = 0; bogus < count; bogus++)
	{
		cache = (DRGN_FontCache*)gfc_list_get_nth(_fontManager.cache, bogus);

		if (!cache)
		{
			continue;
		}

		if (gfc_strlcmp(text, cache->text) != 0)
		{
			continue;
		}

		if (cache->style != style)
		{
			continue;
		}

		if (!gfc_color_cmp(color, cache->color))
		{
			continue;
		}

		return (cache);
	}

	return NULL;
}

void drgn_fontAddCached(const char* text, DRGN_FontStyles style, Color color, SDL_Texture* texture, Vector2D size)
{
	DRGN_FontCache* cache;
	size_t textLength;

	if (!text || !texture)
	{
		return;
	}

	cache = gfc_allocate_array(sizeof(DRGN_FontCache), 1);

	if (!cache)
	{
		slog("could not allocate for the cache");
		return;
	}
	cache->texture = texture;
	vector2d_copy(cache->size, size);
	cache->style = style;
	textLength = strlen(text) + 1;
	cache->text = gfc_allocate_array(sizeof(char), textLength);
	strncpy(cache->text, text, textLength);
	gfc_color_copy(cache->color, color);
	cache->timestamp = SDL_GetTicks();
	gfc_list_append(_fontManager.cache, cache);
}

void drgn_fontCacheListFree()
{
	Uint32 now;
	DRGN_FontCache* cache;
	int count;

	now = SDL_GetTicks();
	count = gfc_list_get_count(_fontManager.cache);

	for (int bogus = count - 1; bogus >= 0; bogus--)
	{
		cache = gfc_list_get_nth(_fontManager.cache, bogus);

		if (!cache)
		{
			continue;
		}

		if (now > cache->timestamp + 1000)
		{
			drgn_fontCacheFree(cache);
			gfc_list_delete_nth(_fontManager.cache, bogus);
		}
	}
}

void drgn_fontCacheFree(DRGN_FontCache* cache)
{
	if (!cache)
	{
		return;
	}

	SDL_DestroyTexture(cache->texture);
	free(cache->text);
	free(cache);
}