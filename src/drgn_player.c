#include "simple_logger.h"
#include "drgn_player.h"

DRGN_Entity* drgn_playerNew(DRGN_Entity* army)
{
	DRGN_Entity* self;
	DRGN_Player temp;
	self = drgn_entityNew();

	if (!self)
	{
		slog("No free space for player entity exists");
		return NULL;
	}

	if (!army)
	{
		slog("No army to assign the player");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all("images/cursor.png", 16, 16, 4, 0);
	self->frame = 0;
	self->pos = vector2d(0, 0);
	self->scale = vector2d(4, 4);
	self->color = GFC_COLOR_LIGHTCYAN;
	self->affiliation = DRGN_BLUE;
	self->think = drgn_playerThink;
	self->update = drgn_playerUpdate;
	self->free = drgn_playerFree;
	temp.army = army;
	//temp.test = 5;
	self->data = &temp;
	slog("Player entity spawned");
	//slog("%i", (*(DRGN_Player*)self->data).test);
	return (self);
}

void drgn_playerThink(DRGN_Entity* self)
{
	const Uint8* keys;
	Vector2D dir = { 0 };
	SDL_Event event;

	SDL_PumpEvents();
	keys = SDL_GetKeyboardState(NULL);

	if (!self)
	{
		slog("No player to update");
		return;
	}

	if (keys[SDL_SCANCODE_W])
	{
		dir.y = -1;
	}
	else if (keys[SDL_SCANCODE_S])
	{
		dir.y = 1;
	}
	else if (keys[SDL_SCANCODE_A])
	{
		dir.x = -1;
	}
	else if (keys[SDL_SCANCODE_D])
	{
		dir.x = 1;
	}

	/*if (keys[SDL_SCANCODE_SPACE] && !self->selected && drgn_entityGetSelectionByPosition(self->affiliation, &self->pos, self))
	{
		self->selected = 1;
	}
	else if (keys[SDL_SCANCODE_SPACE] && self->selected && !drgn_entityGetSelectionByPosition(self->affiliation, &self->pos, self))
	{
		self->selected = 0;
	}*/
	SDL_PollEvent(&event);
	if (keys[SDL_SCANCODE_SPACE] && !self->selected && event.type == SDL_KEYDOWN)
	{
		self->selected = 1;
		slog("begin selection");
	}


	vector2d_normalize(&dir);
	vector2d_scale(self->velocity, dir, 3);
	/*Uint32 mx, my;
	Vector2D dir = { 0 };

	if (!self)
	{
		slog("No player to update");
		return;
	}

	SDL_GetMouseState(&mx, &my);

	if (self->pos.x < mx)
	{
		dir.x = 1;
	}
	if (self->pos.x > mx)
	{
		dir.x = -1;
	}
	if (self->pos.y < my)
	{
		dir.y = 1;
	}
	if (self->pos.y > my)
	{
		dir.y = -1;
	}

	vector2d_normalize(&dir);
	vector2d_scale(self->velocity, dir, 3);*/
}

void drgn_playerUpdate(DRGN_Entity* self)
{
	DRGN_Entity* unit;

	if (!self)
	{
		slog("No player to have thoughts");
		return;
	}

	//slog("Player is updating");
	self->frame += 0.05;

	if (self->frame > 4)
	{
		self->frame = 0;
	}

	vector2d_add(self->pos, self->pos, self->velocity);

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
		//unit = drgn_playerCheckSelectionByPosition(self, (*(DRGN_Player*)self->data).army);
		//unit = drgn_entityGetSelectionByPosition(self->affiliation, &self->pos);

		/*if (unit)
		{
			slog("Selected a unit");
		}
		else
		{
			self->selected = 0;
		}*/
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