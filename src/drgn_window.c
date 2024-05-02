#include "simple_logger.h"
#include "gfc_types.h"
#include "drgn_window.h"
#include "drgn_camera.h"

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

static SJson* _windowJson = NULL;
static SJson* _windowObjects = NULL;

void drgn_windowFileClose()
{
	if (_windowJson)
	{
		slog("freeing window file");
		sj_free(_windowJson);
	}

	_windowJson = NULL;
	_windowObjects = NULL;

	slog("freed window file");
}

void drgn_windowFileInit(const char* name)
{
	if (!name)
	{
		slog("no filename given");
		return;
	}

	_windowJson = sj_load(name);

	if (!_windowJson)
	{
		slog("window json file not found for file %s", name);
		return;
	}

	_windowObjects = sj_object_get_value(_windowJson, "windows");

	if (!_windowObjects)
	{
		slog("could not find window objects for file %s", name);
		sj_free(_windowJson);
		_windowJson = NULL;
		return;
	}

	atexit(drgn_windowFileClose);
}

SJson* drgn_windowGetDefByName(const char* name)
{
	SJson* iter;
	int count;
	const char* iterName;

	if (!name)
	{
		slog("no name given");
		return NULL;
	}

	if (!_windowObjects)
	{
		slog("no window objects to search through, try initiating the file");
		return NULL;
	}

	count = sj_array_get_count(_windowObjects);

	for (int bogus = 0; bogus < count; bogus++)
	{
		iter = sj_array_get_nth(_windowObjects, bogus);

		if (!iter)
		{
			continue;
		}

		iterName = sj_object_get_value_as_string(iter, "name");

		if (!iterName)
		{
			continue;
		}

		if (gfc_strlcmp(name, iterName) == 0)
		{
			return (iter);
		}
	}
}

/*
* @purpose a container for window management
*/
typedef struct
{
	DRGN_Window* windows; //list of all windows currently in use
	Uint32 max; //max number of elements in the list
}
DRGN_WindowManager;

static DRGN_WindowManager _windows = { 0 }; //local global window manager

void drgn_windowFreeAll()
{
	for (int bogus = 0; bogus < _windows.max; bogus++)
	{
		if (!_windows.windows[bogus]._inuse)
		{
			continue;
		}

		slog("freeing window %i", bogus);
		drgn_windowFree(&_windows.windows[bogus]);
	}
}

void drgn_windowManagerFree()
{
	drgn_windowFreeAll();

	if (!_windows.windows)
	{
		slog("No window list to free");
		return;
	}

	free(_windows.windows);
	memset(&_windows, 0, sizeof(DRGN_WindowManager));
	slog("Freed window manager successfully");
}

void drgn_windowManagerNew(Uint32 max)
{
	if (_windows.windows)
	{
		slog("Window manager already exists");
		return;
	}

	if (!max)
	{
		slog("Cannot allocate 0 windows");
		return;
	}

	_windows.windows = gfc_allocate_array(sizeof(DRGN_Window), max);

	if (!_windows.windows)
	{
		slog("Failed to allocate window space");
		return;
	}

	_windows.max = max;
	slog("Window manager created");
	
	atexit(drgn_windowManagerFree);
}

void drgn_windowUpdateAll()
{
	for (int bogus = 0; bogus < _windows.max; bogus++)
	{
		if (!_windows.windows[bogus]._inuse)
		{
			continue;
		}

		//slog("window number %i with inuse flag %i", bogus, _windows.windows[bogus]._inuse);
		drgn_windowUpdate(&_windows.windows[bogus]);
	}
}

void drgn_windowDrawAll()
{
	for (int bogus = 0; bogus < _windows.max; bogus++)
	{
		if (!_windows.windows[bogus]._inuse)
		{
			continue;
		}

		drgn_windowDraw(&_windows.windows[bogus]);
	}
}

DRGN_Window* drgn_windowNew(const char* name, DRGN_ButtonAction action, DRGN_Entity* parent)
{
	SJson* window;
	SJson* windels;
	SJson* element;
	Vector2D pos;
	Vector2D scale;
	DRGN_Windel** elements;
	int check, count;
	const char* windelType;

	if (!name)
	{
		slog("no window name given");
		return NULL;
	}
	window = drgn_windowGetDefByName(name);

	if (!window)
	{
		slog("could not find window of name %s", name);
		sj_free(window);
		return NULL;
	}

	check = sj_object_get_value_as_float(window, "posX", &pos.x);

	if (!check)
	{
		pos.x = 0;
	}

	check = sj_object_get_value_as_float(window, "posY", &pos.y);

	if (!check)
	{
		pos.y = 0;
	}

	check = sj_object_get_value_as_float(window, "scaleX", &scale.x);

	if (!check)
	{
		scale.x = 1;
	}

	check = sj_object_get_value_as_float(window, "scaleY", &scale.y);

	if (!check)
	{
		scale.y = 1;
	}

	windels = sj_object_get_value(window, "windels");
	count = sj_array_get_count(windels);
	elements = gfc_allocate_array(sizeof(DRGN_Windel*), count);

	for (int bogus = 0; bogus < count; bogus++)
	{
		element = sj_array_get_nth(windels, bogus);

		if (!element)
		{
			continue;
		}

		windelType = sj_object_get_value_as_string(element, "type");

		if (gfc_strlcmp(windelType, "text") == 0)
		{
			elements[bogus] = drgn_windelTextNew(element, pos);
			slog("text created");
		}
		else if (gfc_strlcmp(windelType, "sprite") == 0)
		{
			elements[bogus] = drgn_windelSpriteNew(element, pos);
			slog("sprite created");
		}
		else if (gfc_strlcmp(windelType, "button") == 0)
		{
			elements[bogus] = drgn_windelButtonNew(element, pos, action, parent);
			slog("button created");
		}
		else
		{
			slog("invalid windel type or no windel type provided");
			//sj_free(window);
			return NULL;
		}
	}

	for (int bogus = 0; bogus < _windows.max; bogus++)
	{
		if (_windows.windows[bogus]._inuse)
		{
			continue;
		}

		memset(&_windows.windows[bogus], 0, sizeof(DRGN_Window));
		_windows.windows[bogus]._inuse = 1;
		vector2d_copy(_windows.windows[bogus].pos, pos);
		vector2d_copy(_windows.windows[bogus].scale, scale);
		sj_object_get_value_as_uint8(window, "offset", &_windows.windows[bogus].offsetPos);
		_windows.windows[bogus].elements = elements;
		_windows.windows[bogus].elementsNum = count;
		//sj_free(window);
		return (&_windows.windows[bogus]);
	}

	slog("Could not find free memory space for window");
	return NULL;
}

void drgn_windowFree(DRGN_Window* self)
{

	if (!self || !self->_inuse)
	{
		return;
	}

	self->_inuse = 0;

	if (!self->elementsNum || !self->elements)
	{
		slog("no windels to free");
		//free(self);
		return;
	}

	for (int bogus = 0; bogus < self->elementsNum; bogus++)
	{
		if (!self->elements[bogus])
		{
			continue;
		}

		drgn_windelFree(self->elements[bogus]);
	}

	free(self->elements);
	//slog("freeing window");
}

void drgn_windowUpdate(DRGN_Window* self)
{
	if (!self)
	{
		return;
	}

	for (int bogus = 0; bogus < self->elementsNum; bogus++)
	{
		if (!self->elements[bogus])
		{
			continue;
		}

		drgn_windelUpdate(self->elements[bogus]);
	}
}

void drgn_windowDraw(DRGN_Window* self)
{
	Vector2D offset;
	Vector2D pos;

	if (!self)
	{
		return;
	}

	if (self->offsetPos)
	{
		offset = drgn_cameraGetOffset();
		vector2d_add(pos, self->pos, offset);
		drgn_windowChangePosition(self, pos);
		self->offsetPos = 0;
	}

	for (int bogus = 0; bogus < self->elementsNum; bogus++)
	{
		if (!self->elements[bogus])
		{
			continue;
		}

		drgn_windelDraw(self->elements[bogus]);
	}
}

void drgn_windowChangePosition(DRGN_Window* self, Vector2D changePos)
{
	DRGN_Windel* windel;

	if (!self || !self->elements || !self->elementsNum)
	{
		return;
	}

	vector2d_sub(changePos, self->pos, changePos);
	vector2d_sub(self->pos, self->pos, changePos);

	for (int bogus = 0; bogus < self->elementsNum; bogus++)
	{
		windel = self->elements[bogus];

		if (!windel)
		{
			continue;
		}

		vector2d_sub(windel->pos, windel->pos, changePos);
	}
}

DRGN_Windel* drgn_windowGetPositionByName(Vector2D pos, const char* name)
{
	if (!name)
	{
		return NULL;
	}

	for (int bogus = 0; bogus < _windows.max; bogus++)
	{
		if (!_windows.windows[bogus]._inuse)
		{
			continue;
		}

		if (!_windows.windows[bogus].elementsNum || !_windows.windows[bogus].elements)
		{
			continue;
		}

		for (int bogus2 = 0; bogus2 < _windows.windows[bogus].elementsNum; bogus2++)
		{
			if (!_windows.windows[bogus].elements[bogus2])
			{
				continue;
			}

			if (gfc_strlcmp(_windows.windows[bogus].elements[bogus2]->name, name) == 0)
			{
				slog("Cursor y: %f, window y: %f", pos.y, _windows.windows[bogus].pos.y);

				if (pos.y == _windows.windows[bogus].pos.y)
				{
					return (_windows.windows[bogus].elements[bogus2]);
				}
			}
		}
	}

	return NULL;
}

void drgn_windowAssignActionByName(DRGN_Window* window, const char* name, DRGN_ButtonAction action)
{
	if (!window)
	{
		return;
	}

	if (!name)
	{
		slog("no name given");
		return;
	}

	if (!window->elementsNum || !window->elements)
	{
		return;
	}

	for (int bogus = 0; bogus < window->elementsNum; bogus++)
	{
		if (!window->elements[bogus])
		{
			continue;
		}

		if (gfc_strlcmp(window->elements[bogus]->name, "name") == 0)
		{
			drgn_windelButtonAssignAction(window->elements[bogus], action);
		}
	}
}