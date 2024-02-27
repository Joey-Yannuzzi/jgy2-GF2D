#include "simple_logger.h"
#include "drgn_move.h"

DRGN_Entity* drgn_moveNew(Color color, Vector2D pos, const char* sprite, int width, int height)
{
	DRGN_Entity* self;

	self = drgn_entityNew();

	if (!self)
	{
		slog("No memory available for movement square");
		return NULL;
	}

	self->think = drgn_moveThink;
	self->update = drgn_moveUpdate;
	self->free = drgn_moveFree;
	self->color = color;
	vector2d_copy(self->pos, pos);
	self->sprite = gf2d_sprite_load_all(sprite, width, height, 1, 0);
	self->scale = vector2d(1, 1);
	self->affiliation = DRGN_TILE;
	return (self);
}

void drgn_moveThink(DRGN_Entity* self)
{

}

void drgn_moveUpdate(DRGN_Entity* self)
{

}

void drgn_moveFree(DRGN_Entity* self)
{

}

void drgn_moveManagerInit(Uint32 max)
{
	if (_moveManager.moveList)
	{
		slog("Already exists");
		return;
	}

	if (!max)
	{
		return;
	}

	_moveManager.moveList = gfc_allocate_array(sizeof(DRGN_Entity), max);

	if (!_moveManager.moveList)
	{
		slog("Failed to allocate");
		return;
	}

	_moveManager.max = max;
	atexit(drgn_moveManagerClose);
}

void drgn_moveManagerClose()
{
	for (int bogus = 0; bogus < _moveManager.max; bogus++)
	{
		if (!_moveManager.moveList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityFree(&_moveManager.moveList[bogus]);
	}

	free(_moveManager.moveList);
}