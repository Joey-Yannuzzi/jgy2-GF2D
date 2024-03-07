#include "simple_logger.h"
#include "drgn_window.h"
#include "drgn_font.h"

DRGN_Window* drgn_windowNew(List* data, Vector2D pos)
{
	if (!_windowManager.windowList)
	{
		slog("No window manager initialized");
		return NULL;
	}
	for (int bogus = 0; bogus < _windowManager.max; bogus++)
	{
		if (_windowManager.windowList[bogus]._inuse)
		{
			continue;
		}

		memset(&_windowManager.windowList[bogus], 0, sizeof(DRGN_Window));
		_windowManager.windowList[bogus].window = gf2d_sprite_load_all("images/tiles/move.png", 64, 64, 1, 0);
		_windowManager.windowList[bogus].data = gfc_list_copy(data);
		_windowManager.windowList[bogus].color = gfc_color8(0, 0, 0, 100);
		vector2d_copy(_windowManager.windowList[bogus].pos, pos);
		_windowManager.windowList[bogus]._inuse = 1;
		_windowManager.windowList[bogus].window->surface = gf2d_graphics_create_surface(64, 64);
		_windowManager.windowList[bogus].window->surface = gf2d_graphics_screen_convert(&_windowManager.windowList[bogus].window->surface);
		_windowManager.windowList[bogus].window->texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(), _windowManager.windowList[bogus].window->surface);
		return (&_windowManager.windowList[bogus]);
	}

	slog("Maximum windows reached");
	return NULL;
}

void drgn_windowFree(DRGN_Window* self)
{
	int count;
	const char* element;

	if (!self)
	{
		return;
	}

	if (self->window)
	{
		gf2d_sprite_free(self->window);
	}

	count = gfc_list_get_count(self->data);

	for (int bogus = 0; bogus < count; bogus++)
	{
		element = gfc_list_get_nth(self->data, bogus);

		if (!element)
		{
			continue;
		}

		free(element);
	}

	gfc_list_delete(self->data);
	self->_inuse = 0;
	free(self);
}

void drgn_windowManagerNew(Uint32 max)
{
	if (_windowManager.windowList)
	{
		slog("A window manager already exists");
		return;
	}

	if (!max)
	{
		slog("Cannot allocate 0 windows, defaulting to 1");
		max = 1;
	}

	_windowManager.windowList = gfc_allocate_array(sizeof(DRGN_Window), max);

	if (!_windowManager.windowList)
	{
		slog("Error allocating the array");
		return;
	}

	_windowManager.max = max;
	slog("Window system initalized");

	atexit(drgn_windowManagerFree);
}

void drgn_windowManagerFree()
{
	if (!_windowManager.windowList)
	{
		slog("No list to free");
		return;
	}

	for (int bogus = 0; bogus < _windowManager.max; bogus++)
	{
		if (!_windowManager.windowList[bogus]._inuse)
		{
			continue;
		}

		drgn_windowFree(&_windowManager.windowList[bogus]);
	}

	free(_windowManager.windowList);
	memset(&_windowManager, 0, sizeof(DRGN_WindowManager));
}

void drgn_windowDraw(DRGN_Window* self)
{
	SDL_Rect rect;

	if (!self)
	{
		return;
	}

	if (!self->window || !self->window->texture)
	{
		return;
	}

	rect.x = self->pos.x;
	rect.y = self->pos.y;
	rect.w = self->window->surface->w;
	rect.h = self->window->surface->h;
	SDL_RenderCopy(gf2d_graphics_get_renderer(), self->window->texture, NULL, &rect);
}

void drgn_windowManagerDraw()
{
	for (int bogus = 0; bogus < _windowManager.max; bogus++)
	{
		if (!_windowManager.windowList[bogus]._inuse)
		{
			continue;
		}

		drgn_windowDraw(&_windowManager.windowList[bogus]);
	}
}