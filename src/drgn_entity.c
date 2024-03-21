#include "simple_logger.h"
#include "drgn_camera.h"
#include "drgn_entity.h"
#include "drgn_font.h"

typedef struct
{
	DRGN_Entity* entList; //pointer list to all active entities
	Uint32 entMax; //maximum number of active entities
}
DRGN_EntityManager;

static DRGN_EntityManager _entManager = { 0 }; //local global entity manager

void drgn_entitySystemInit(Uint32 max)
{
	if (_entManager.entList)
	{
		slog("Entity manager already exists");
		return;
	}

	if (!max)
	{
		slog("Cannot allocate 0 entities");
		return;
	}

	_entManager.entList = gfc_allocate_array(sizeof(DRGN_Entity), max);

	if (!_entManager.entList)
	{
		slog("Failed to allocate to global entity list");
		return;
	}

	_entManager.entMax = max;
	slog("Entity system initialized");

	atexit(drgn_entitySystemClose);
}

void drgn_entitySystemClose()
{
	drgn_entityCleanAll(NULL);

	if (!_entManager.entList)
	{
		slog("No entity manager exists to close");
		return;
	}
	free(_entManager.entList);
	memset(&_entManager, 0, sizeof(DRGN_EntityManager));
}

void drgn_entityCleanAll(DRGN_Entity* ignore)
{
	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (&_entManager.entList[bogus] == ignore)
		{
			continue;
		}

		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		slog("Freeing entity number %i", bogus);
		drgn_entityFree(&_entManager.entList[bogus]);
	}
}

DRGN_Entity* drgn_entityNew()
{
	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		memset(&_entManager.entList[bogus], 0, sizeof(DRGN_Entity));
		_entManager.entList[bogus]._inuse = 1;
		//TODO: set default color/scale

		//slog("creating entity %i", bogus);
		return (&_entManager.entList[bogus]);
	}

	slog("Maximum entities reached");
	return NULL;
}

void drgn_entityFree(DRGN_Entity* self)
{
	if (!self)
	{
		slog("No entity to free");
		return;
	}

	if (self->sprite)
	{
		gf2d_sprite_free(self->sprite);
	}
	self->_inuse = 0;
	//TODO: don't forget to free anything that gets allocated

	if (self->free)
	{
		self->free(self);
	}

	slog("successfully freed entity");
}

void drgn_entityThink(DRGN_Entity* self)
{
	if (!self)
	{
		slog("No entity to have thoughts");
		return;
	}

	//TODO: any common think operations for all entities should be added here

	if (self->think)
	{
		self->think(self);
	}
}
void drgn_entitySystemThink()
{
	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityThink(&_entManager.entList[bogus]);
	}
}
void drgn_entityUpdate(DRGN_Entity* self)
{
	if (!self)
	{
		slog("No entity to update");
		return;
	}

	//TODO: any common update operations for all entities should be added here

	if (self->update)
	{
		self->update(self);
	}
}

void drgn_entitySystemUpdate()
{
	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityUpdate(&_entManager.entList[bogus]);
	}
}

void drgn_entityDraw(DRGN_Entity* self, enum DRGN_Affiliation affiliation)
{
	Vector2D offset, pos;

	if (!self)
	{
		slog("No entity to draw");
		return;
	}

	if (self->win)
	{
		drgn_fontDraw("You Win!", DRGN_LARGE_FONT, GFC_COLOR_CYAN, vector2d(self->pos.x, self->pos.y), NULL);
	}

	if (self->inactive)
	{
		return;
	}

	if (self->affiliation != affiliation)
	{
		return;
	}

	offset = drgn_cameraGetOffset();
	vector2d_add(pos, offset, self->pos);

	if (self->sprite)
	{
		gf2d_sprite_render(self->sprite, pos, &self->scale, NULL, NULL, NULL, &self->color, NULL, (Uint32)self->frame);

		if (self->affiliation == DRGN_UI && !self->offset)
		{
			gf2d_sprite_render(self->sprite, self->pos, &self->scale, NULL, NULL, NULL, NULL, NULL, (Uint32)self->frame);
		}
		else if (self->affiliation == DRGN_UI && self->offset)
		{
			vector2d_copy(self->offsetVal, offset);
			gf2d_sprite_render(self->sprite, pos, &self->scale, NULL, NULL, NULL, NULL, NULL, (Uint32)self->frame);
		}
	}

	if (self->draw)
	{
		self->draw(self);
	}
}

void drgn_entitySystemDraw()
{
	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityDraw(&_entManager.entList[bogus], DRGN_TILE);
	}

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityDraw(&_entManager.entList[bogus], DRGN_DEFAULT);
	}

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityDraw(&_entManager.entList[bogus], DRGN_BLUE);
	}

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityDraw(&_entManager.entList[bogus], DRGN_RED);
	}

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityDraw(&_entManager.entList[bogus], DRGN_GREEN);
	}

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityDraw(&_entManager.entList[bogus], DRGN_CURSOR);
	}

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		drgn_entityDraw(&_entManager.entList[bogus], DRGN_UI);
	}
}

DRGN_Entity* drgn_entityGetUnitsByAffiliation(enum DRGN_Affiliation affiliation)
{
	DRGN_Entity list[] = { 0 };
	int units = 0;

	if (!affiliation)
	{
		slog("No affiliation to assign to army");
		return NULL;
	}

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		if (_entManager.entList[bogus].affiliation == affiliation)
		{
			list[units++] = _entManager.entList[bogus];
			slog("found unit %i", bogus);
		}
	}

	if (units < 1)
	{
		return NULL;
	}

	return (&list);
}

DRGN_Entity* drgn_entityGetSelectionByPosition(enum DRGN_Affiliation affiliation, Vector2D pos, DRGN_Entity* self)
{;

	for (int bogus = 0; bogus < _entManager.entMax; bogus++)
	{
		if (!_entManager.entList[bogus]._inuse)
		{
			continue;
		}

		if (&_entManager.entList[bogus] == self)
		{
			continue;
		}

		if (_entManager.entList[bogus].affiliation == affiliation && (_entManager.entList[bogus].pos.x == pos.x && _entManager.entList[bogus].pos.y == pos.y))
		{
			return (&_entManager.entList[bogus]);
		}
	}

	return NULL;
}