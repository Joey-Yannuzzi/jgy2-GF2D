#include "simple_logger.h"
#include "drgn_terrain.h"

DRGN_Entity* drgn_terrainNew(int tile, Vector2D pos)
{
	DRGN_Entity* self;
	DRGN_Terrain* terrain;
	SJson* terrainDef;
	SJson* obj;
	SJson* temp;
	const char* name;
	float moveValue;
	int count;

	self = drgn_entityNew();

	if (!self)
	{
		slog("No free space for terrain exists");
		return NULL;
	}

	self->think = drgn_terrainThink;
	self->update = drgn_terrainUpdate;
	self->free = drgn_terrainFree;

	if (!tile)
	{
		slog("invalid tile %i", tile);
		drgn_entityFree(self);
		return NULL;
	}

	terrainDef = drgn_terrainGetDefByNumber(tile);

	if (!terrainDef)
	{
		slog("Terrain could not be found with index %i", tile);
		drgn_entityFree(self);
		return NULL;
	}

	vector2d_copy(self->pos, pos);
	self->affiliation = DRGN_DEFAULT; //terrain do not have an affiliation therefore the default (0) is used

	terrain = gfc_allocate_array(sizeof(DRGN_Terrain), 1);

	if (!terrain)
	{
		slog("Failed to allocate memory for terrain with index %i", tile);
		drgn_entityFree(self);
		return NULL;
	}

	name = sj_object_get_value_as_string(terrainDef, "displayName");

	if (!name)
	{
		slog("Failed to find display name for terrain with index %i", tile);
		drgn_entityFree(self);
		return NULL;
	}

	terrain->name = name;
	terrain->tile = tile;
	sj_object_get_value_as_float(terrainDef, "moveValue", &moveValue);

	if (!moveValue)
	{
		slog("No valid move value assigned to terrain with name %s", terrain->name);
		drgn_entityFree(self);
		return NULL;
	}

	terrain->moveValue = moveValue;
	sj_object_get_value_as_int(terrainDef, "avoidBonus", &terrain->avoidBonus);
	sj_object_get_value_as_int(terrainDef, "defBonus", &terrain->defBonus);

	obj = sj_object_get_value(terrainDef, "ignoreClass");

	if (obj)
	{
		count = sj_array_get_count(obj);
		terrain->ignoreClass = gfc_allocate_array(sizeof(const char*), count);

		for (int bogus = 0; bogus < count; bogus++)
		{
			temp = sj_array_get_nth(obj, bogus);

			if (!temp)
			{
				continue;
			}

			terrain->ignoreClass[bogus] = sj_get_string_value(temp);
		}

		terrain->ignoreCount = count;
	}

	self->data = terrain;
	slog("%s terrain successfully created", terrain->name);
	return (self);
}

void drgn_terrainThink(DRGN_Entity* self)
{

}

void drgn_terrainUpdate(DRGN_Entity* self)
{

}

void drgn_terrainFree(DRGN_Entity* self)
{
	DRGN_Terrain* terrain;

	if (!self || !self->data)
	{
		return;
	}

	terrain = (DRGN_Terrain*)self->data;

	if (terrain->ignoreClass)
	{
		free(terrain->ignoreClass);
	}

	free(terrain);
}

void drgn_terrainFileInit(const char* name)
{
	if (!name)
	{
		slog("No file name given");
		return;
	}

	_terrainJson = sj_load(name);

	if (!_terrainJson)
	{
		slog("failed to load json object with name", name);
		return;
	}

	_terrainList = sj_object_get_value(_terrainJson, "terrain");

	if (!_terrainList)
	{
		slog("No terrain specified in this file with name %s", name);
		sj_free(_terrainJson);
		_terrainJson = NULL;
		return;
	}

	atexit(drgn_terrainFileFree);
}

void drgn_terrainFileFree()
{
	if (_terrainJson)
	{
		sj_free(_terrainJson);
	}

	_terrainJson = NULL;
	_terrainList = NULL;
}

SJson* drgn_terrainGetDefByNumber(int index)
{
	SJson* terrain;
	int count;
	int tile = 0;

	if (index < 1)
	{
		slog("Invalid terrain tile index %i", index);
		return NULL;
	}

	terrain = sj_array_get_nth(_terrainList, index - 1); //subtract one because index starts at 1 and array starts at 0
	if (terrain)
	{
		sj_object_get_value_as_int(terrain, "tile", &tile);
	}

	if (tile == index);
	{
		slog("Found terrain without loop");
		return (terrain);
	}

	count = sj_array_get_count(_terrainList);

	for (int bogus = 0; bogus < count; bogus++)
	{
		terrain = sj_array_get_nth(_terrainList, bogus);

		if (!terrain)
		{
			continue;
		}

		sj_object_get_value_as_int(terrain, "tile", &tile);

		if (tile == index)
		{
			slog("Found terrain in loop");
			return (terrain);
		}
	}

	slog("Could not find terrain with tile value %i", index);
	return NULL;
}