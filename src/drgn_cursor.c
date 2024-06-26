#include "simple_logger.h"
#include "drgn_cursor.h"
#include "drgn_window.h"
#include "drgn_unit.h"

DRGN_Entity* drgn_cursorNew(Vector2D pos, Vector2D upperBounds, Vector2D lowerBounds, DRGN_Entity* parent, int max)
{
	DRGN_Entity* self;
	DRGN_Cursor* cursor;

	self = drgn_entityNew();

	if (!self)
	{
		return NULL;
	}

	self->think = drgn_cursorThink;
	self->update = drgn_cursorUpdate;
	self->free = drgn_cursorFree;
	vector2d_copy(self->pos, pos);
	self->scale = vector2d(1, 1);
	self->affiliation = DRGN_UI;
	self->offset = 1;
	self->offsetVal = vector2d(0, 0);
	self->sprite = gf2d_sprite_load_all("images/windowArrow.png", 32, 32, 1, 0);
	cursor = gfc_allocate_array(sizeof(DRGN_Cursor), 1);
	vector2d_copy(cursor->lowerBounds, lowerBounds);
	vector2d_copy(cursor->upperBounds, upperBounds);
	cursor->parent = parent;
	//cursor->windows = windows;
	cursor->curr = 0;
	cursor->max = max;
	self->data = cursor;
	return (self);
}

void drgn_cursorThink(DRGN_Entity* self)
{
	const Uint8* keys;
	Vector2D dir = { 0 };
	SDL_Event event;
	Vector2D offset = { 0 };
	DRGN_Cursor* cursor;

	SDL_PumpEvents();
	keys = SDL_GetKeyboardState(NULL);
	SDL_PollEvent(&event);

	if (!self || !self->data)
	{
		return;
	}

	cursor = (DRGN_Cursor*)self->data;

	if (cursor->pressed)
	{
		return;
	}

	if (keys[SDL_SCANCODE_W])
	{
		dir.y = -1;
		cursor->curr--;
	}
	else if (keys[SDL_SCANCODE_S])
	{
		dir.y = 1;
		cursor->curr++;
	}

	if (keys[SDL_SCANCODE_E] && !cursor->pressed)
	{
		cursor->pressed = 1;
	}

	vector2d_normalize(&dir);

	if (dir.x)
	{
		offset.x = self->scale.x * self->sprite->frame_w * dir.x;
	}
	if (dir.y)
	{
		offset.y = self->scale.y * self->sprite->frame_h * dir.y;
	}

	vector2d_copy(self->velocity, offset);
}

void drgn_cursorUpdate(DRGN_Entity* self)
{
	DRGN_Cursor* cursor;
	DRGN_Windel* temp;
	DRGN_WindelButton* button;
	DRGN_Entity* ent;
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		return;
	}

	//slog("Cursor: x: %f, y: %f", self->pos.x, self->pos.y);

	cursor = (DRGN_Cursor*)self->data;

	cursor->frameskip++;

	if (cursor->frameskip % 8 > 0)
	{
		return;
	}

	if (cursor->curr < 0)
	{
		cursor->curr = cursor->max + cursor->curr;
	}
	if (cursor->curr >= cursor->max)
	{
		cursor->curr = cursor->curr - cursor->max;
	}

	if (cursor->pressed)
	{
		//temp = drgn_windowGetPositionByName(vector2d(self->pos.x + 31, self->pos.y), "commandButton");
		unit = (DRGN_Unit*)cursor->parent->data;
		temp = unit->menuWindow[cursor->curr]->elements[2];

		if (temp && temp->data)
		{
			button = (DRGN_WindelButton*)temp->data;

			if (button)
			{
				button->pushed = 1;
			}
		}

		cursor->pressed = 0;
	}

	vector2d_add(self->pos, self->pos, self->velocity);

	if (self->pos.y > cursor->lowerBounds.y)
	{
		self->pos.y = cursor->upperBounds.y;
	}
	if (self->pos.y < cursor->upperBounds.y)
	{
		self->pos.y = cursor->lowerBounds.y;
	}
}

void drgn_cursorFree(DRGN_Entity* self)
{
	if (!self)
	{
		return;
	}
}