#include "simple_logger.h"
#include "drgn_entity.h"

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

	gf2d_sprite_free(self->sprite);
	self->_inuse = 0;
	//TODO: don't forget to free anything that gets allocated

	if (self->free)
	{
		self->free(self->data);
	}
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

void drgn_entityDraw(DRGN_Entity* self)
{
	if (!self)
	{
		slog("No entity to draw");
		return;
	}

	if (self->sprite)
	{
		gf2d_sprite_render(self->sprite, self->pos, &self->scale, NULL, NULL, NULL, &self->color, NULL, (Uint32)self->frame);
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

		drgn_entityDraw(&_entManager.entList[bogus]);
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
			slog("found unit");
		}
	}

	if (units < 1)
	{
		return NULL;
	}

	return (&list);
}