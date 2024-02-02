#include "simple_logger.h"
#include "drgn_player.h"

DRGN_Entity* drgn_playerNew()
{
	DRGN_Entity* self;
	self = drgn_entityNew();

	if (!self)
	{
		slog("No free space for player entity exists");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all("images/ed210.png", 128, 128, 16, 0);
	self->frame = 0;
	self->pos = vector2d(0, 0);
	self->think = drgn_playerThink;
	self->update = drgn_playerUpdate;
	self->free = drgn_playerFree;
	slog("Player entity spawned");
	return (self);
}

void drgn_playerThink(DRGN_Entity* self)
{
	Uint32 mx, my;
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
	vector2d_scale(self->velocity, dir, 3);
}

void drgn_playerUpdate(DRGN_Entity* self)
{
	if (!self)
	{
		slog("No player to have thoughts");
		return;
	}

	//slog("Player is updating");
	self->frame += 0.1;

	if (self->frame > 151)
	{
		self->frame = 0;
	}

	vector2d_add(self->pos, self->pos, self->velocity);
}

void drgn_playerFree(DRGN_Entity* self)
{
	if (!self)
	{
		slog("No player to be freed");
		return;
	}
}