#include "simple_logger.h"
#include "gfc_types.h"
#include "drgn_window.h"

/*
DRGN_Entity* drgn_windowNew(char* texts, const char* sprite, Uint32 width, Uint32 height, Vector2D pos, DRGN_Entity* curr)
{
	DRGN_Entity* self;
	DRGN_Window* window;
	size_t textLen;

	self = drgn_entityNew();

	if (!self)
	{
		slog("failed to create entity");
		return NULL;
	}

	self->think = drgn_windowThink;
	self->update = drgn_windowUpdate;
	self->free = drgn_windowFree;
	self->draw = drgn_windowDraw;
	self->offsetVal = vector2d(0, 0);

	if (!texts)
	{
		slog("No texts for this window");
		drgn_entityFree(self);
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all(sprite, width, height, 1, 0);

	if (!self->sprite)
	{
		slog("No sprite was loaded with filename %s", sprite);
		drgn_entityFree(self);
		return NULL;
	}

	vector2d_copy(self->pos, pos);
	self->scale = vector2d(1, 1);
	self->affiliation = DRGN_UI;
	window = gfc_allocate_array(sizeof(DRGN_Window), 1);

	if (!window)
	{
		slog("window could not be created");
		drgn_entityFree(self);
		return NULL;
	}

	textLen = strlen(texts) + 1;
	window->texts = gfc_allocate_array(sizeof(char), textLen);
	strncpy(window->texts, texts, textLen);
	self->data = window;
	self->curr = curr;

	//slog("Drew window");
	return (self);
}

void drgn_windowFree(DRGN_Entity* self)
{
	DRGN_Window* window;

	if (!self || !self->data)
	{
		return;
	}

	window = (DRGN_Window*)self->data;

	//free(window->texts);
	free(window);
}

void drgn_windowThink(DRGN_Entity* self)
{

}

void drgn_windowUpdate(DRGN_Entity* self)
{
	DRGN_Window* window;
	DRGN_Unit* unit;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	window = (DRGN_Window*)self->data;
	unit = (DRGN_Unit*)self->curr->data;

	if (self->selected)
	{
		unit->currentAction = drgn_windowMenuItemFromText(self);

		if (unit->currentAction)
		{
			drgn_unitMenu(self->curr);
		}
		else
		{
			slog("do not consume item");
			self->selected = 0;
		}
		//slog("Selected %s", window->texts);
	}
}

void drgn_windowDraw(DRGN_Entity* self)
{
	DRGN_Window* window;
	Vector2D pos;
	Vector2D vert;

	if (!self || !self->data)
	{
		return;
	}

	vector2d_add(pos, self->pos, self->offsetVal);
	window = (DRGN_Window*)self->data;
	vert = vector2d(self->sprite->frame_w, self->sprite->frame_h);
	drgn_fontDraw(window->texts, DRGN_SMALL_FONT, GFC_COLOR_BLACK, pos, &vert);
}

DRGN_Action drgn_windowMenuItemFromText(DRGN_Entity* self)
{
	DRGN_Window* window;
	DRGN_Unit* unit;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return (DRGN_NO_ACTION);
	}

	window = (DRGN_Window*)self->data;
	unit = (DRGN_Unit*)self->curr->data;

	if (gfc_strlcmp(window->texts, "Wait") == 0)
	{
		return(DRGN_WAIT);
	}
	if (gfc_strlcmp(window->texts, "Seize") == 0)
	{
		return (DRGN_SEIZE);
	}
	if (gfc_strlcmp(window->texts, "Talk") == 0)
	{
		return (DRGN_TALK);
	}
	if (gfc_strlcmp(window->texts, "Attack") == 0)
	{
		if (drgn_inventoryCheckItemTypeInInventory(unit->inventory, DRGN_ARCANE))
		{
			return (DRGN_MAGIC_ATTACK);
		}

		return (DRGN_MELEE_ATTACK);
	}
	if (gfc_strlcmp(window->texts, "Heal") == 0)
	{
		return (DRGN_HEAL);
	}
	if (gfc_strlcmp(window->texts, "Item") == 0 && (unit->inventory->itemList[unit->inventory->equipped].type == DRGN_POTION || unit->inventory->itemList[unit->inventory->equipped].type == DRGN_STAT_BOOSTER))
	{
		slog("consuming item");
		return (DRGN_ITEM);
	}
	if (gfc_strlcmp(window->texts, "Trade") == 0)
	{
		return (DRGN_TRADE);
	}
	if (gfc_strlcmp(window->texts, "Rescue") == 0)
	{
		return (DRGN_RESCUE);
	}
	if (gfc_strlcmp(window->texts, "Transfer") == 0)
	{
		return (DRGN_TRANSFER);
	}
	if (gfc_strlcmp(window->texts, "Drop") == 0)
	{
		return (DRGN_DROP);
	}

	return (DRGN_NO_ACTION);
}*/

typedef struct
{
	DRGN_Window* windows; //list of all windows currently in use
	Uint32 max; //max number of elements in the list
}
DRGN_WindowManager;

static DRGN_WindowManager _windows = { 0 }; //local global window manager

DRGN_Window* drgn_windowNew(Vector2D pos, Vector2D scale, Uint8 offsetPos, DRGN_Windel** elements)
{
	for (int bogus = 0; bogus < _windows.max; bogus++)
	{
		if (_windows.windows[bogus]._inuse)
		{
			continue;
		}

		memset(&_windows.windows[bogus], 0, sizeof(DRGN_Window));
		_windows.windows[bogus]._inuse = 1;
		_windows.windows[bogus].pos = pos;
		_windows.windows[bogus].scale = scale;
		_windows.windows[bogus].offsetPos = offsetPos;
		_windows.windows[bogus].elements = elements;
		return (&_windows.windows[bogus]);
	}

	slog("Could not find free memory space for window");
	return NULL;
}