#include "simple_logger.h"
#include "drgn_selector.h"

DRGN_Selector* drgn_selectorNew(DRGN_Window* window)
{
	DRGN_Selector* selector;
	int count = 0;
	int fail = 0;

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

	for (int bogus = 0; bogus < window->elementsNum; bogus++)
	{
		if (!window->elements[bogus] || !window->elements[bogus]->selectable)
		{
			continue;
		}

		count++;
	}

	selector->max = count;
	selector->buttons = gfc_allocate_array(sizeof(DRGN_Windel*), count);
	selector->texts = gfc_allocate_array(sizeof(DRGN_Windel*), count);

	for (int bogus = 0; bogus < window->elementsNum; bogus++)
	{
		if (!window->elements[bogus] || !window->elements[bogus]->selectable)
		{
			fail++;
			continue;
		}

		selector->buttons[bogus - fail] = window->elements[bogus];
		selector->texts[bogus - fail] = drgn_windowGetWindelByPosition(window, window->elements[bogus], window->elements[bogus]->pos);
	}

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
	//SDL_PollEvent(&event);

	if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_A])
	{
		selector->dir = -1;
	}
	else if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_D])
	{
		selector->dir = 1;
	}
	else
	{
		selector->dir = 0;
	}

	if (keys[SDL_SCANCODE_SPACE])
	{
		selector->pressed = 1;
	}
}

void drgn_selectorUpdate(DRGN_Selector* selector)
{
	DRGN_WindelButton* button;

	if (!selector || !selector->parent)
	{
		return;
	}

	selector->frameskip++;

	if (selector->frameskip % 8 > 0)
	{
		return;
	}

	selector->current += selector->dir;

	if (selector->current < 0)
	{
		selector->current = selector->max + selector->current;
	}
	
	if (selector->current >= selector->max)
	{
		selector->current = selector->current - selector->max;
	}

	for (int bogus = 0; bogus < selector->max; bogus++)
	{
		if (!selector->texts[bogus])
		{
			slog("something is wrong");
			continue;
		}

		if (selector->texts[bogus] == selector->texts[selector->current])
		{
			selector->texts[bogus]->color = GFC_COLOR_GREY;
		}
		else
		{
			selector->texts[bogus]->color = GFC_COLOR_WHITE;
		}
	}

	if (selector->pressed)
	{
		button = (DRGN_WindelButton*)selector->buttons[selector->current]->data;
		button->pushed = 1;
		selector->pressed = 0;
	}
}

void drgn_selectorFree(DRGN_Selector* selector)
{
	if (!selector)
	{
		return;
	}
}

void drgn_selectorFindNewSelection(DRGN_Selector* selector)
{
	int count = 0;
	int fail = 0;

	selector->current = 0;

	for (int bogus = 0; bogus < selector->parent->elementsNum; bogus++)
	{
		if (!selector->parent->elements[bogus] || !selector->parent->elements[bogus]->selectable)
		{
			continue;
		}

		count++;
	}

	selector->max = count;
	selector->buttons = gfc_allocate_array(sizeof(DRGN_Windel*), count);
	selector->texts = gfc_allocate_array(sizeof(DRGN_Windel*), count);

	for (int bogus = 0; bogus < selector->parent->elementsNum; bogus++)
	{
		if (!selector->parent->elements[bogus] || !selector->parent->elements[bogus]->selectable)
		{
			fail++;
			continue;
		}

		selector->buttons[bogus - fail] = selector->parent->elements[bogus];
		selector->texts[bogus - fail] = drgn_windowGetWindelByPosition(selector->parent, selector->parent->elements[bogus], selector->parent->elements[bogus]->pos);
	}
}