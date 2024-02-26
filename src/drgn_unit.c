#include "simple_logger.h"
#include "drgn_unit.h"
#include "drgn_terrain.h"
#include "drgn_world.h"

DRGN_Entity* drgn_unitNew(const char* name, const char* inventory[], enum DRGN_Affiliation affiliation, Vector2D pos)
{
	DRGN_Entity* self;
	DRGN_Unit* unit;
	SJson* unitDef;
	SJson* stats;
	SJson* stat;
	int frame, width, height, framesPerLine, count;
	const char* displayName;
	const char* sprite;

	self = drgn_entityNew();

	if (!self)
	{
		slog("No free space for unit exists");
		return NULL;
	}

	self->think = drgn_unitThink;
	self->update = drgn_unitUpdate;
	self->free = drgn_unitFree;

	if (!name)
	{
		slog("no name given");
		drgn_entityFree(self);
		return NULL;
	}

	unitDef = drgn_unitGetDefByName(name);

	if (!unitDef)
	{
		slog("no unit found with name %s", name);
		drgn_entityFree(self);
		return NULL;
	}

	sprite = sj_object_get_value_as_string(unitDef, "sprite");

	if (!sprite)
	{
		slog("no sprite specified for unit named %s", name);
		drgn_entityFree(self);
		return NULL;
	}

	sj_object_get_value_as_int(unitDef, "width", &width);
	sj_object_get_value_as_int(unitDef, "height", &height);

	if (!width || !height)
	{
		slog("invalid dimensions of %i, %i", width, height);
		drgn_entityFree(self);
		return NULL;
	}

	sj_object_get_value_as_int(unitDef, "framesPerLine", &framesPerLine);

	if (!framesPerLine)
	{
		slog("invalid frame number per line");
		drgn_entityFree(self);
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all(sprite, width, height, framesPerLine, 0);
	sj_object_get_value_as_int(unitDef, "frame", &self->frame);
	vector2d_copy(self->pos, pos);
	self->scale = vector2d(1, 1);

	switch (affiliation)
	{
	case DRGN_DEFAULT:
		slog("no army to put player in");
		drgn_entityFree(self);
		return NULL;
	case DRGN_BLUE:
		self->color = GFC_COLOR_BLUE;
		break;
	case DRGN_RED:
		self->color = GFC_COLOR_RED;
		break;
	case DRGN_GREEN:
		self->color = GFC_COLOR_RED;
		break;
	default:
		slog("no affiliation for unit with name %s", name);
		drgn_entityFree(self);
		return NULL;
	}

	self->affiliation = affiliation;

	unit = gfc_allocate_array(sizeof(DRGN_Unit), 1);

	if (!unit)
	{
		slog("failed to allocate memory for unit");
		drgn_entityFree(self);
		return NULL;
	}

	stats = sj_object_get_value(unitDef, "stats");

	if (!stats)
	{
		slog("no stats to populate to unit");
		drgn_entityFree(self);
		return NULL;
	}

	count = sj_array_get_count(stats);

	for (int bogus = 0; bogus < count; bogus++)
	{
		stat = sj_array_get_nth(stats, bogus);

		if (!stat)
		{
			continue;
		}

		sj_get_integer_value(stat, &unit->stats[bogus]);
	}

	stats = sj_object_get_value(unitDef, "growths");

	if (!stats)
	{
		slog("no growths to populate unit");
		drgn_entityFree(self);
		return NULL;
	}

	count = sj_array_get_count(stats);

	for (int bogus = 0; bogus < count; bogus++)
	{
		stat = sj_array_get_nth(stats, bogus);

		if (!stat)
		{
			continue;
		}

		sj_get_integer_value(stat, &unit->growths[bogus]);
	}

	stats = sj_object_get_value(unitDef, "weaponTypes");

	if (!stats)
	{
		slog("No weapon ranks for unit");
		drgn_entityFree(self);
		return NULL;
	}

	count = sj_array_get_count(stats);

	for (int bogus = 0; bogus < count; bogus++)
	{
		stat = sj_array_get_nth(stats, bogus);

		if (!stat)
		{
			continue;
		}

		sj_get_integer_value(stat, &unit->weaponLvls[bogus]);
	}

	unit->moveTile = NULL;
	unit->attackTitle = NULL;
	unit->animate = 1;
	slog("unit here");
	displayName = sj_object_get_value_as_string(unitDef, "displayName");

	if (displayName)
	{
		slog("Unit has unique display name");
		unit->name = displayName;
	}

	unit->inventory = drgn_inventoryNew(inventory, 5);

	if (!unit->inventory)
	{
		slog("failed to create inventory");
		drgn_entityFree(self);
		return NULL;
	}

	self->data = unit;
	return (self);
}

void drgn_unitThink(DRGN_Entity* self)
{
	DRGN_Entity* terrain;
	DRGN_Terrain* terrainData;
	DRGN_Unit* unit;
	DRGN_Entity* check;

	if (!self)
	{
		return;
	}

	terrain = drgn_entityGetSelectionByPosition(0, self->pos, self);

	if (terrain)
	{
		terrainData = (DRGN_Terrain*)terrain->data;

		if (terrainData)
		{
			//slog("Unit is on terrain %s", terrainData->name);
		}
	}

	if (!self->selected)
	{
		return;
	}

	if (!self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (unit->moveDrawn)
	{
		return;
	}

	check = drgn_unitCalcMove(self, unit->stats[9], self->pos);
	self->selected = 0;
	unit->moveDrawn = 1;
	self->curr = check;
}

void drgn_unitUpdate(DRGN_Entity* self)
{
	self->frame += 0.05;

	if (self->frame > 4)
	{
		self->frame = 0;
	}
}

void drgn_unitFree(DRGN_Entity* self)
{
	DRGN_Unit* unit;

	if (!self)
	{
		slog("no unit to free");
		return;
	}

	if (!self->data)
	{
		return;
	}

	//return;

	unit = (DRGN_Unit*)self->data;
	slog("%s", unit->name);
	
	if (!unit->inventory)
	{
		slog("No unit or inventory to free");
		free(unit);
		return;
	}

	slog("after if");
	drgn_inventoryFree(unit->inventory);
	slog("after inventory");
	free(unit);
}

DRGN_Entity* drgn_unitMoveNew(DRGN_Entity* self, Vector2D pos)
{
	DRGN_Entity* move;
	DRGN_Unit* unit;

	if (!self)
	{
		return NULL;
	}

	Color color = gfc_color8(0, 0, 255, 100);
	move = drgn_moveNew(color, pos, "images/tiles/move.png", self->sprite->frame_w, self->sprite->frame_h);

	if (!move)
	{
		return NULL;
	}

	return (move);
}

DRGN_Entity* drgn_unitCalcMove(DRGN_Entity* self, int move, Vector2D pos)
{
	DRGN_Entity* moveEnt = NULL;
	DRGN_Entity* nextMove = NULL;
	int x = pos.x;
	int y = pos.y;

	//Base Cases:
	//Out of movement: true
	//Enemy is found: true
	//Out of bounds: true
	//
	//Recusive Cases
	//Blank square is found

	if (!self)
	{
		return NULL;
	}

	if (move < 0)
	{
		slog("out of movement");
		return (self);
	}
	if (drgn_entityGetSelectionByPosition(DRGN_RED, pos, self))
	{
		slog("hit an enemy");
		return (self);
	}
	if (x < 0 || y < 0 || x >(drgn_worldGetWidth() * 64) || y >(drgn_worldGetHeight() * 64))
	{
		slog("hit a wall");
		slog("%i of %i", x, drgn_worldGetWidth() * 64);
		slog("%i of %i", y, drgn_worldGetHeight() * 64);
		return self;
	}

	if (drgn_entityGetSelectionByPosition(DRGN_TILE, pos, self))
	{
		slog("already colored this tile");

		if (drgn_entityGetSelectionByPosition(DRGN_TILE, vector2d(x + 64, y), self) && drgn_entityGetSelectionByPosition(DRGN_TILE, vector2d(x - 64, y), self) && drgn_entityGetSelectionByPosition(DRGN_TILE, vector2d(x , y + 64), self) && drgn_entityGetSelectionByPosition(DRGN_TILE, vector2d(x, y - 64), self))
		{
			return (self);
		}
		else
		{
			moveEnt = self;
		}
	}
	else
	{
		moveEnt = drgn_unitMoveNew(self, pos);
	}

	slog("%i, %i", x, y);

	if ((x-64) >= 0)
	{
		slog("x - 1");
		slog("movement left: %i", move);
		nextMove = drgn_unitCalcMove(moveEnt, move - 1, vector2d(x - 64, pos.y));
	}
	if ((x + 64) <= (drgn_worldGetWidth() * 64))
	{
		slog("x + 1");
		slog("movement left: %i", move);
		nextMove = drgn_unitCalcMove(moveEnt, move - 1, vector2d(x + 64, pos.y));
	}
	if ((y - 64) >= 0)
	{
		slog("y - 1");
		slog("movement left: %i", move);
		nextMove = drgn_unitCalcMove(moveEnt, move - 1, vector2d(pos.x, y - 64));
	}
	if ((y + 64) <= (drgn_worldGetHeight() * 64))
	{
		slog("y + 1");
		slog("movement left: %i", move);
		nextMove = drgn_unitCalcMove(moveEnt, move - 1, vector2d(pos.x, y + 64));
	}

	if (!nextMove)
	{
		return (moveEnt);
	}
	
	return (nextMove);
}

void drgn_unitFileInit(const char* name)
{
	if (!name)
	{
		slog("no name given for generic units");
		return;
	}

	_unitJson = sj_load(name);

	if (!_unitJson)
	{
		slog("No file with name %s", name);
		return;
	}

	_unitIds = sj_object_get_value(_unitJson, "units");

	if (!_unitIds)
	{
		slog("no units specified in file with name %s", name);
		sj_free(_unitJson);
		_unitJson = NULL;
		return;
	}

	atexit(drgn_unitFileFree);
}

void drgn_unitFileFree()
{
	if (_unitJson)
	{
		sj_free(_unitJson);
	}

	_unitJson = NULL;
	_unitIds = NULL;
}

SJson* drgn_unitGetDefByName(const char* name)
{
	SJson* unit;
	int count;
	const char* unitName;

	if (!name)
	{
		slog("No name given for unit");
		return NULL;
	}

	if (!_unitIds)
	{
		slog("No unit defs to grab from, please initialize the file");
		return NULL;
	}

	count = sj_array_get_count(_unitIds);

	for (int bogus = 0; bogus < count; bogus++)
	{
		unit = sj_array_get_nth(_unitIds, bogus);

		if (!unit)
		{
			continue;
		}

		unitName = sj_object_get_value_as_string(unit, "name");

		if (!unitName)
		{
			continue;
		}

		if (gfc_strlcmp(name, unitName) == 0)
		{
			return (unit);
		}
	}

	slog("Could not find unit with name %s", name);
	return NULL;
}