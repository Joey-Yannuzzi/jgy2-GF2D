#include "simple_logger.h"
#include "drgn_unit.h"

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
		slog("no generic units specified in file with name %s", name);
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