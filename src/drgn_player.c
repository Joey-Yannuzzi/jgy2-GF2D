#include "simple_logger.h"
#include "drgn_player.h"
#include "drgn_camera.h"
#include "drgn_terrain.h"

DRGN_Entity* drgn_playerNew()
{
	DRGN_Entity* self;
	self = drgn_entityNew();

	if (!self)
	{
		slog("No free space for player entity exists");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all("images/newCursor.png", 64, 64, 4, 0);
	self->frame = 0;
	self->pos = vector2d(0, 0);
	self->scale = vector2d(1, 1);
	self->color = GFC_COLOR_LIGHTCYAN;
	self->affiliation = DRGN_BLUE;
	self->think = drgn_playerThink;
	self->update = drgn_playerUpdate;
	self->free = drgn_playerFree;
	slog("Player entity spawned");
	//slog("%i", (*(DRGN_Player*)self->data).test);
	return (self);
}

void drgn_playerThink(DRGN_Entity* self)
{
	const Uint8* keys;
	Vector2D dir = { 0 };
	SDL_Event event;
	Vector2D offset = { 0 };

	SDL_PumpEvents();
	keys = SDL_GetKeyboardState(NULL);
	SDL_PollEvent(&event);

	if (!self)
	{
		slog("No player to update");
		return;
	}

	if (keys[SDL_SCANCODE_W] && event.type == SDL_KEYDOWN)
	{
		dir.y = -1;
	}
	else if (keys[SDL_SCANCODE_S] && event.type == SDL_KEYDOWN)
	{
		dir.y = 1;
	}
	else if (keys[SDL_SCANCODE_A] && event.type == SDL_KEYDOWN)
	{
		dir.x = -1;
	}
	else if (keys[SDL_SCANCODE_D] && event.type == SDL_KEYDOWN)
	{
		dir.x = 1;
	}

	if (keys[SDL_SCANCODE_SPACE] && !self->selected && event.type == SDL_KEYDOWN)
	{
		self->selected = 1;
		slog("begin selection");
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

void drgn_playerUpdate(DRGN_Entity* self)
{
	DRGN_Entity* unit;
	DRGN_Entity* terrain;
	DRGN_Terrain* terrainData;
	Rect bounds;

	if (!self)
	{
		slog("No player to have thoughts");
		return;
	}

	self->frame += 0.05;

	if (self->frame > 4)
	{
		self->frame = 0;
	}

	vector2d_add(self->pos, self->pos, self->velocity);
	bounds = drgn_cameraGetBounds();

	if ((self->pos.x + (self->scale.x * self->sprite->frame_w)) > (bounds.w + bounds.x))
	{
		self->pos.x = (bounds.w + bounds.x) - (self->scale.x * self->sprite->frame_w);
	}
	if ((self->pos.y + (self->scale.y * self->sprite->frame_h)) > (bounds.h + bounds.y))
	{
		self->pos.y = (bounds.h + bounds.y) - (self->scale.y * self->sprite->frame_h);
	}
	if (self->pos.x < bounds.x)
	{
		self->pos.x = bounds.x;
	}
	if (self->pos.y < bounds.y)
	{
		self->pos.y = bounds.y;
	}

	drgn_cameraCenterOn(self->pos);

	if (self->selected  && !self->curr)
	{
		//slog("unit selected");
		unit = drgn_entityGetSelectionByPosition(self->affiliation, self->pos, self);
		self->selected = 0;

		if (!unit)
		{
			slog("No unit to select");
			return;
		}

		self->curr = unit;
		slog("Unit selected");
	}
	else if (self->selected && self->curr && (self->pos.x != self->curr->pos.x || self->pos.y != self->curr->pos.y))
	{
		slog("unit unselected");
		self->curr->pos = self->pos;
		self->curr->color = GFC_COLOR_BLUE;
		self->curr = NULL;
		self->selected = 0;
	}
	else
	{
		self->selected = 0;
	}

	terrain = drgn_entityGetSelectionByPosition(0, self->pos, self);

	if (!terrain)
	{
		slog("Player not on any known terrain");
		return;
	}

	terrainData = (DRGN_Terrain*)terrain->data;

	if (!terrainData)
	{
		slog("No data on terrain");
		return;
	}

	slog("Player is on terrain %s", terrainData->name);
}

void drgn_playerFree(DRGN_Entity* self)
{
	if (!self)
	{
		slog("No player to be freed");
		return;
	}
}

DRGN_Entity* drgn_playerCheckSelectionByPosition(DRGN_Entity* selector, DRGN_Entity* army)
{
	int bogus = 0;

	while (&army[bogus])
	{
		if (!army[bogus]._inuse)
		{
			bogus++;
			continue;
		}

		if (selector->affiliation != army->affiliation)
		{
			bogus++;
			continue;
		}

		if  (army->pos.x == selector->pos.x && army->pos.y == selector->pos.y)
		{
			return (&army[bogus]);
		}

		bogus++;
	}

	return NULL;
}

void drgn_playerCheckBounds(DRGN_Entity* self, Rect bounds)
{
	if ((self->pos.x + (self->scale.x * self->sprite->frame_w)) > (bounds.w + bounds.x))
	{
		self->pos.x = (bounds.w + bounds.x) - (self->scale.x * self->sprite->frame_w);
	}
	if ((self->pos.y + (self->scale.y * self->sprite->frame_h)) > (bounds.h + bounds.y))
	{
		self->pos.y = (bounds.h + bounds.y) - (self->scale.y * self->sprite->frame_h);
	}
	if (self->pos.x < bounds.x)
	{
		self->pos.x = bounds.x;
	}
	if (self->pos.y < bounds.y)
	{
		self->pos.y = bounds.y;
	}
}