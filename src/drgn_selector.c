#include "simple_logger.h"
#include "drgn_selector.h"

DRGN_Selector* drgn_selectorNew(DRGN_Window* window)
{
	DRGN_Selector* selector;

	if (!window || !window->elements)
	{
		slog("No window or windels given");
		return NULL;
	}

	selector = gfc_allocate_array(sizeof(DRGN_Selector), 1);

	if (!selector)
	{
		slog("failed to make the selector");
		return NULL;
	}

	selector->parent = window;
	selector->current = 0;
	return (selector);
}

void drgn_selectorThink(DRGN_Selector* selector)
{
	const Uint8* keys;
	SDL_Event event;

	if (!selector || !selector->parent)
	{
		return;
	}

	SDL_PumpEvents();
	keys = SDL_GetKeyboardState(NULL);
	SDL_PollEvent(&event);

	if ((keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_A]) && event.type == SDL_KEYDOWN)
	{
		selector->dir = -1;
	}
	else if ((keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_D]) && event.type == SDL_KEYDOWN)
	{
		selector->dir = 1;
	}
	else
	{
		selector->dir = 0;
	}
}

void drgn_selectorUpdate(DRGN_Selector* selector)
{
	DRGN_Windel* windel;

	if (!selector || !selector->parent)
	{
		return;
	}

	selector->current += selector->dir;

	if (selector->current < 0)
	{
		selector->current = selector->parent->elements[selector->parent->elementsNum - 1];
	}
	
	if (selector->current >= selector->parent->elementsNum)
	{
		selector->current = selector->parent->elements[0];
	}

	if (!selector->parent->elements[selector->current]->selectable)
	{
		selector->current += selector->dir;
	}

	windel = drgn_windowGetWindelByPosition(selector->parent, selector->parent->elements[selector->current], selector->parent->elements[selector->current]->pos);

	if (selector->parent->elements[selector->current]->selectable && windel)
	{
		windel->color = GFC_COLOR_GREY;
	}
}

void drgn_selectorFree(DRGN_Selector* selector)
{
	if (!selector)
	{
		return;
	}
}