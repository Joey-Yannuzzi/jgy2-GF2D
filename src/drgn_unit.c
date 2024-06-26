#include "simple_logger.h"
#include "drgn_shop.h"
#include "drgn_unit.h"
#include "drgn_terrain.h"
#include "drgn_world.h"
#include "drgn_cursor.h"
#include "drgn_player.h"

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
	const char* class;
	const char* names[] = { "spellBook", "healStaff", "mediumPotion", "largePotion", "smallPotion" };

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
		self->color = GFC_COLOR_GREEN;
		break;
	default:
		slog("no affiliation for unit with name %s", name);
		drgn_entityFree(self);
		return NULL;
	}

	self->affiliation = affiliation;
	self->colorSet = 1;

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

	class = sj_object_get_value_as_string(unitDef, "class");

	if (!class)
	{
		slog("No class assigned to unit");
		return NULL;
	}

	unit->class = class;

	if (gfc_strlcmp(unit->class, "arcanist") == 0)
	{
		unit->inventory = drgn_inventoryNew(names, 5);
		slog("arcanist inventory filled");
	}
	else
	{
		unit->inventory = drgn_inventoryNew(inventory, 5);
	}

	if (!unit->inventory)
	{
		slog("failed to create inventory");
		drgn_entityFree(self);
		return NULL;
	}

	unit->moveTotal = ((unit->stats[9] * 2) + 1) * ((unit->stats[9] * 2) + 1);
	unit->moveMap = gfc_allocate_array(sizeof(Uint8), unit->moveTotal);
	unit->active = 1;
	unit->menuWindow = gfc_allocate_array(sizeof(DRGN_Window*), 8);
	unit->currentHP = unit->stats[1];
	unit->currentHP--;
	unit->inventory->equipped = 0;

	self->data = unit;
	return (self);
}

void drgn_unitThink(DRGN_Entity* self)
{
	DRGN_Entity* terrain;
	DRGN_Terrain* terrainData;
	DRGN_Unit* unit;
	DRGN_Entity* check;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (unit->rescued)
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
}

void drgn_unitUpdate(DRGN_Entity* self)
{
	DRGN_Unit* unit;
	char* text;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (unit->rescued)
	{
		return;
	}

	if (!unit->active)
	{
		self->frame = 0;
		return;
	}

	self->frame += 0.05;

	if (self->frame > 4)
	{
		self->frame = 0;
	}

	if (self->selected)
	{
		slog("Array size %i", unit->moveTotal);
		unit->moveTiles = gfc_allocate_array(sizeof(DRGN_Entity), unit->moveTotal);
		drgn_unitCalcMove(self, unit->stats[9] + 1, self->pos, (unit->moveTotal - 1) / 2);
		self->selected = 0;
		free(unit->moveMap);
		unit->moveMap = NULL;
		unit->moveMap = gfc_allocate_array(sizeof(Uint8), unit->moveTotal);
	}

	if (unit->selector)
	{
		drgn_unitActionShop(self);
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

	if (!unit->selector)
	{
		free(unit);
		return;
	}

	free(unit);
}

DRGN_Entity* drgn_unitMoveNew(DRGN_Entity* self, Vector2D pos, int index)
{
	DRGN_Entity* move;
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		return NULL;
	}

	unit = (DRGN_Unit*)self->data;

	if (unit->moveTiles[index])
	{
		return NULL;
	}
	Color color = gfc_color8(0, 0, 255, 100);
	move = drgn_moveNew(color, pos, "images/tiles/move.png", self->sprite->frame_w, self->sprite->frame_h);

	if (!move)
	{
		return NULL;
	}

	unit->moveTiles[index] = move;

	return (move);
}

void drgn_unitCalcMove(DRGN_Entity* self, float move, Vector2D pos, int index)
{
	DRGN_Unit* unit;
	DRGN_Entity* terrainSuper;
	DRGN_Terrain* terrain;
	int x = pos.x;
	int y = pos.y;
	int offset;
	float moveValue = 1;

	//Base Cases:
	//Out of movement: true
	//Enemy is found: true
	//Out of bounds: true
	//
	//Recusive Cases
	//Blank square is found

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (drgn_entityGetSelectionByPosition(DRGN_RED, pos, self))
	{
		return;
	}
	if (x < 0 || y < 0 || x >(drgn_worldGetWidth() * 64) || y >(drgn_worldGetHeight() * 64))
	{
		return;
	}

	terrainSuper = drgn_entityGetSelectionByPosition(DRGN_DEFAULT, pos, self);
	if (terrainSuper && terrainSuper->data)
	{
		terrain = (DRGN_Terrain*)terrainSuper->data;
		moveValue = terrain->moveValue;

		if (terrain->ignoreClass)
		{
			for (int bogus = 0; bogus < terrain->ignoreCount; bogus++)
			{
				if (gfc_strlcmp(terrain->ignoreClass[bogus], unit->class) == 0)
				{
					return;
				}
			}
		}
	}

	move -= moveValue;

	if (move < 0)
	{
		return;
	}

	if (index < 0 || index > unit->moveTotal)
	{
		slog("Error index out of bounds at index %i", index);
		return;
	}
	unit->moveMap[index]++;

	if (unit->moveMap[index] == 1 && !drgn_entityGetSelectionByPosition(DRGN_BLUE, pos, self))
	{
		drgn_unitMoveNew(self, pos, index);
	}

	offset = (unit->stats[9] * 2) + 1;

	if ((x-64) >= 0)
	{
		drgn_unitCalcMove(self, move, vector2d(x - 64, pos.y), index - 1);
	}
	if ((x + 64) <= (drgn_worldGetWidth() * 64))
	{
		drgn_unitCalcMove(self, move, vector2d(x + 64, pos.y), index + 1);
	}
	if ((y - 64) >= 0)
	{
		drgn_unitCalcMove(self, move, vector2d(pos.x, y - 64), index - offset);
	}
	if ((y + 64) <= (drgn_worldGetHeight() * 64))
	{
		drgn_unitCalcMove(self, move, vector2d(pos.x, y + 64), index + offset);
	}
}

void drgn_unitMoveFree(DRGN_Entity* self)
{
	DRGN_Unit* unit;
	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	for (int bogus = 0; bogus < unit->moveTotal; bogus++)
	{
		if (!unit->moveTiles)
		{
			continue;
		}

		drgn_entityFree(unit->moveTiles[bogus]);
	}

	free(unit->moveTiles);
	unit->moveTiles = NULL;
	unit->moveTiles = gfc_allocate_array(sizeof(DRGN_Entity), unit->moveTotal);
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

	slog("freed unit file successfully");
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

void drgn_unitMenu(DRGN_Entity* self)
{
	DRGN_Unit* unit;
	int bogus = 0;
	DRGN_Entity* left;
	DRGN_Entity* right;
	DRGN_Entity* up;
	DRGN_Entity* down;
	DRGN_Unit* unitLeft;
	DRGN_Unit* unitRight;
	DRGN_Unit* unitUp;
	DRGN_Unit* unitDown;
	DRGN_Entity* temp;
	DRGN_Terrain* terrain;
	DRGN_ButtonAction action;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	switch (unit->currentAction)
	{
	case DRGN_MOVE:

		temp = drgn_entityGetSelectionByPosition(DRGN_DEFAULT, self->pos, self);

		if (temp && temp->data)
		{
			terrain = (DRGN_Terrain*)temp->data;

			if (gfc_strlcmp(terrain->name, "Flag") == 0)
			{
				unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_SEIZE, self, 0, 0);
				drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
				drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Seize");
				bogus++;
			}
			else if (gfc_strlcmp(terrain->name, "Item Shop") == 0)
			{
				unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_ITEM_SHOP, self, 0, 0);
				drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
				drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Item Shop");
				bogus++;
			}
		}

		right = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x + 64, self->pos.y), self);
		left = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x - 64, self->pos.y), self);
		down = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x, self->pos.y + 64), self);
		up = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x, self->pos.y - 64), self);

		if (right || left || down || up)
		{
			unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_TALK, self, 0, 0);
			drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
			drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Talk");
			bogus++;
		}

		if (drgn_inventoryCheckItemTypeInInventory(unit->inventory, DRGN_ARCANE))
		{
			slog("magic attack");
			action = DRGN_BUTTON_MAGIC_ATTACK;
			right = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x + 128, self->pos.y), self);
			left = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x - 128, self->pos.y), self);
			down = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y + 128), self);
			up = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y - 128), self);
		}
		else
		{
			slog("melee attack");
			action = DRGN_BUTTON_MELEE_ATTACK;
			right = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x + 64, self->pos.y), self);
			left = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x - 64, self->pos.y), self);
			down = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y + 64), self);
			up = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y - 64), self);
		}
		unitRight = NULL;
		unitLeft = NULL;
		unitUp = NULL;
		unitDown = NULL;

		if (right || left || up || down)
		{
			if (right)
			{
				unitRight = (DRGN_Unit*)right->data;
			}
			if (left)
			{
				unitLeft = (DRGN_Unit*)left->data;
			}
			if (up)
			{
				unitUp = (DRGN_Unit*)up->data;
			}
			if (down)
			{
				unitDown = (DRGN_Unit*)down->data;
			}

			if (unitRight || unitLeft || unitUp || unitDown)
			{
				unit->menuWindow[bogus] = drgn_windowNew("commandWindow", action, self, 0, 0);
				drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
				drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Attack");
				bogus++;
			}
		}

		right = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self);
		left = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self);
		down = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self);
		up = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self);
		unitRight = NULL;
		unitLeft = NULL;
		unitUp = NULL;
		unitDown = NULL;

		if (right || left || down || up)
		{
			if (right)
			{
				unitRight = (DRGN_Unit*)right->data;
			}
			if (left)
			{
				unitLeft = (DRGN_Unit*)left->data;
			}
			if (up)
			{
				unitUp = (DRGN_Unit*)up->data;
			}
			if (down)
			{
				unitDown = (DRGN_Unit*)down->data;
			}

			if (drgn_inventoryCheckItemTypeInInventory(unit->inventory, DRGN_DIVINE) &&((unitRight && unitRight->currentHP < unitRight->stats[1]) || (unitLeft && unitLeft->currentHP < unitLeft->stats[1]) || (unitUp && unitUp->currentHP < unitUp->stats[1]) || (unitDown && unitDown->currentHP < unitDown->stats[1])))
			{
				unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_HEAL, self, 0, 0);
				drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
				drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Heal");
				bogus++;
			}
		}
		if (unit->inventory->curr > 0)
		{
			unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_ITEM, self, 0, 0);
			drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
			drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Item");
			bogus++;
		}

		right = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self);
		left = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self);
		down = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self);
		up = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self);
		unitRight = NULL;
		unitLeft = NULL;
		unitUp = NULL;
		unitDown = NULL;

		if  (right || left || down || up)
		{
			if (right)
			{
				unitRight = (DRGN_Unit*)right->data;
			}
			if (left)
			{
				unitLeft = (DRGN_Unit*)left->data;
			}
			if (up)
			{
				unitUp = (DRGN_Unit*)up->data;
			}
			if (down)
			{
				unitDown = (DRGN_Unit*)down->data;
			}

			if (unit->inventory->curr > 0 || ((unitRight && unitRight->inventory->curr > 0) || (unitLeft && unitLeft->inventory->curr > 0) || (unitUp && unitUp->inventory->curr > 0) || (unitDown && unitDown->inventory->curr > 0)))
			{
				unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_TRADE, self, 0, 0);
				drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
				drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Trade");
				bogus++;
			}
		}

		right = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self);
		left = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self);
		down = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self);
		up = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self);
		unitRight = NULL;
		unitLeft = NULL;
		unitUp = NULL;
		unitDown = NULL;

		if (right || left || down || up)
		{
			if (right)
			{
				unitRight = (DRGN_Unit*)right->data;
			}
			if (left)
			{
				unitLeft = (DRGN_Unit*)left->data;
			}
			if (up)
			{
				unitUp = (DRGN_Unit*)up->data;
			}
			if (down)
			{
				unitDown = (DRGN_Unit*)down->data;
			}

			if (!unit->rescuedUnit && ((unitRight && unit->stats[10] > unitRight->stats[10] && !unitRight->rescuedUnit) || (unitLeft && unit->stats[10] > unitLeft->stats[10] && !unitLeft->rescuedUnit) || (unitUp && unit->stats[10] > unitUp->stats[10] && !unitUp->rescuedUnit) || (unitDown && unit->stats[10] > unitDown->stats[10] && !unitDown->rescuedUnit)))
			{
				unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_RESCUE, self, 0, 0);
				drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
				drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Rescue");
				bogus++;
			}
		}

		right = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self);
		left = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self);
		down = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self);
		up = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self);
		unitRight = NULL;
		unitLeft = NULL;
		unitUp = NULL;
		unitDown = NULL;

		if (right || left || down || up)
		{
			if (right)
			{
				unitRight = (DRGN_Unit*)right->data;
			}
			if (left)
			{
				unitLeft = (DRGN_Unit*)left->data;
			}
			if (up)
			{
				unitUp = (DRGN_Unit*)up->data;
			}
			if (down)
			{
				unitDown = (DRGN_Unit*)down->data;
			}

			if (unit->rescuedUnit && ((unitRight && unit->stats[10] <= unitRight->stats[10] && !unitRight->rescuedUnit) || (unitLeft && unit->stats[10] <= unitLeft->stats[10] && !unitLeft->rescuedUnit) || (unitUp && unit->stats[10] <= unitUp->stats[10] && !unitUp->rescuedUnit) || (unitDown && unit->stats[10] <= unitDown->stats[10] && !unitDown->rescuedUnit)))
			{
				unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_TRANSFER, self, 0, 0);
				drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
				drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Transfer");
				bogus++;
			}
		}
		if (unit->rescuedUnit && (
			!drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x + 64, self->pos.y), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x - 64, self->pos.y), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y + 64), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y - 64), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x + 64, self->pos.y), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x - 64, self->pos.y), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x, self->pos.y + 64), self) ||
			!drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x, self->pos.y - 64), self)))
		{
			unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_DROP, self, 0, 0);
			drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
			drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Drop");
			bogus++;
		}

		unit->menuWindow[bogus] = drgn_windowNew("commandWindow", DRGN_BUTTON_WAIT, self, 0, 0);
		drgn_windowChangePosition(unit->menuWindow[bogus], vector2d(self->pos.x + 96, self->pos.y + (bogus * 32)));
		drgn_windelTextChangeText(unit->menuWindow[bogus]->elements[1], "Wait");
		unit->menuCursor = drgn_cursorNew(vector2d(self->pos.x + 65, self->pos.y), unit->menuWindow[0]->pos, unit->menuWindow[bogus]->pos, self, bogus);
		bogus++;
		unit->menuMax = bogus;
		break;

	case DRGN_WAIT:
		drgn_unitWait(self);
		break;

	case DRGN_SEIZE:
		drgn_unitSeize(self);
		break;

	case DRGN_TALK:
		drgn_unitTalk(self);
		break;
	
	case DRGN_MELEE_ATTACK:
		drgn_unitMeleeAttack(self);
		break;

	case DRGN_MAGIC_ATTACK:
		drgn_unitRangedAttack(self);
		break;

	case DRGN_HEAL:
		drgn_unitHeal(self);
		break;

	case DRGN_ITEM:
		drgn_unitItem(self, &unit->inventory->itemList[unit->inventory->equipped]);
		break;

	case DRGN_TRADE:
		drgn_unitTrade(self);
		break;

	case DRGN_RESCUE:
		drgn_unitRescue(self);
		break;

	case DRGN_TRANSFER:
		//drgn_unitTransfer(self);
		break;

	case DRGN_DROP:
		//drgn_unitDrop(self);
		break;
	case DRGN_ITEM_SHOP:
		drgn_unitItemShop(self);
		break;
	case DRGN_ARMORY:
		break;
	case DRGN_BUY:
		drgn_unitBuy(self);
		break;
	case DRGN_SELL:
		drgn_unitSell(self);
		break;
	default:
		break;
	}
}

void drgn_unitItem(DRGN_Entity* self, DRGN_InventoryItem* item)
{
	DRGN_InventoryItemPotion* potion;
	DRGN_InventoryItemStatBooster* booster;
	DRGN_Unit* unit;
	int healing;

	if (!self || !item || !item->data || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (item->type == DRGN_POTION)
	{
		potion = (DRGN_InventoryItemPotion*)item->data;
		healing = potion->heal;

		if (unit->currentHP >= unit->stats[1])
		{
			return;
		}

		unit->currentHP += healing;

		if (unit->currentHP > unit->stats[1])
		{
			unit->currentHP = unit->stats[1];
		}

		slog("healed for %i health", healing);

	}
	else if (item->type == DRGN_STAT_BOOSTER)
	{
		booster = (DRGN_InventoryItemStatBooster*)item->data;
		unit->stats[booster->stat] += booster->increase;
		slog("increased sta %i by %i points", booster->stat, booster->increase);
	}

	drgn_unitWait(self);
}

void drgn_unitWait(DRGN_Entity* self)
{
	DRGN_Unit* unit;
	DRGN_Player* player;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	self->color = GFC_COLOR_GREY;
	unit->active = 0;
	self->selected = 0;
	drgn_unitMoveFree(self);
	drgn_unitMenuFree(unit);
	drgn_entityFree(unit->menuCursor);
	unit->menuCursor = NULL;
	self->curr->inactive = 0;
	player = (DRGN_Player*)self->curr->data;
	player->targeting = 0;
	player->currentTarget = 0;
	player->totalTargets = 0;
	self->curr->curr = NULL;
	self->curr = NULL;
}

void drgn_unitSeize(DRGN_Entity* self)
{
	drgn_unitWait(self);
	self->win = 1;
	//Win the game as well
}

void drgn_unitTalk(DRGN_Entity* self)
{
	DRGN_Entity* right;
	DRGN_Entity* left;
	DRGN_Entity* up;
	DRGN_Entity* down;
	DRGN_Unit* unit;
	DRGN_Player* player;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	drgn_unitMoveFree(self);
	drgn_entityFree(unit->menuCursor);
	unit->menuCursor = NULL;
	drgn_unitMenuFree(unit);

	right = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x + 64, self->pos.y), self);
	left = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x - 64, self->pos.y), self);
	down = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x, self->pos.y + 64), self);
	up = drgn_entityGetSelectionByPosition(DRGN_GREEN, vector2d(self->pos.x, self->pos.y - 64), self);
	player = (DRGN_Player*)self->curr->data;

	if (right)
	{
		self->curr->pos = right->pos;
		player->targets[player->totalTargets] = right;
		player->totalTargets++;
	}
	if (left)
	{
		self->curr->pos = left->pos;
		player->targets[player->totalTargets] = left;
		player->totalTargets++;
	}
	if (up)
	{
		self->curr->pos = up->pos;
		player->targets[player->totalTargets] = up;
		player->totalTargets++;
	}
	if (down)
	{
		self->curr->pos = down->pos;
		player->targets[player->totalTargets] = down;
		player->totalTargets++;
	}

	player->targeting = 1;
	self->curr->inactive = 0;
	player->pressed = 0;
	self->curr->curr = self;
}

void drgn_unitMeleeAttack(DRGN_Entity* self)
{
	DRGN_Entity* right;
	DRGN_Entity* left;
	DRGN_Entity* up;
	DRGN_Entity* down;
	DRGN_Unit* unit;
	DRGN_Player* player;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	drgn_unitMoveFree(self);
	drgn_entityFree(unit->menuCursor);
	unit->menuCursor = NULL;
	drgn_unitMenuFree(unit);

	right = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x + 64, self->pos.y), self);
	left = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x - 64, self->pos.y), self);
	down = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y + 64), self);
	up = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y - 64), self);
	player = (DRGN_Player*)self->curr->data;

	if (right)
	{
		self->curr->pos = right->pos;
		player->targets[player->totalTargets] = right;
		player->totalTargets++;
	}
	if (left)
	{
		self->curr->pos = left->pos;
		player->targets[player->totalTargets] = left;
		player->totalTargets++;
	}
	if (up)
	{
		self->curr->pos = up->pos;
		player->targets[player->totalTargets] = up;
		player->totalTargets++;
	}
	if (down)
	{
		self->curr->pos = down->pos;
		player->targets[player->totalTargets] = down;
		player->totalTargets++;
	}

	player->targeting = 1;
	self->curr->inactive = 0;
	player->pressed = 0;
	self->curr->curr = self;
}

void drgn_unitRangedAttack(DRGN_Entity* self)
{
	DRGN_Entity* right;
	DRGN_Entity* left;
	DRGN_Entity* up;
	DRGN_Entity* down;
	DRGN_Unit* unit;
	DRGN_Player* player;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	drgn_unitMoveFree(self);
	drgn_entityFree(unit->menuCursor);
	unit->menuCursor = NULL;
	drgn_unitMenuFree(unit);

	right = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x + 128, self->pos.y), self);
	left = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x - 128, self->pos.y), self);
	down = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y + 128), self);
	up = drgn_entityGetSelectionByPosition(DRGN_RED, vector2d(self->pos.x, self->pos.y - 128), self);
	player = (DRGN_Player*)self->curr->data;

	if (right)
	{
		self->curr->pos = right->pos;
		player->targets[player->totalTargets] = right;
		player->totalTargets++;
	}
	if (left)
	{
		self->curr->pos = left->pos;
		player->targets[player->totalTargets] = left;
		player->totalTargets++;
	}
	if (up)
	{
		self->curr->pos = up->pos;
		player->targets[player->totalTargets] = up;
		player->totalTargets++;
	}
	if (down)
	{
		self->curr->pos = down->pos;
		player->targets[player->totalTargets] = down;
		player->totalTargets++;
	}

	player->targeting = 1;
	self->curr->inactive = 0;
	player->pressed = 0;
	self->curr->curr = self;
}

void drgn_unitHeal(DRGN_Entity* self)
{
	DRGN_Entity* right;
	DRGN_Entity* left;
	DRGN_Entity* up;
	DRGN_Entity* down;
	DRGN_Unit* unit;
	DRGN_Player* player;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	drgn_unitMoveFree(self);
	drgn_entityFree(unit->menuCursor);
	unit->menuCursor = NULL;
	drgn_unitMenuFree(unit);

	right = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self);
	left = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self);
	down = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self);
	up = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self);
	player = (DRGN_Player*)self->curr->data;

	if (right)
	{
		self->curr->pos = right->pos;
		player->targets[player->totalTargets] = right;
		player->totalTargets++;
	}
	if (left)
	{
		self->curr->pos = left->pos;
		player->targets[player->totalTargets] = left;
		player->totalTargets++;
	}
	if (up)
	{
		self->curr->pos = up->pos;
		player->targets[player->totalTargets] = up;
		player->totalTargets++;
	}
	if (down)
	{
		self->curr->pos = down->pos;
		player->targets[player->totalTargets] = down;
		player->totalTargets++;
	}

	player->targeting = 1;
	self->curr->inactive = 0;
	player->pressed = 0;
	self->curr->curr = self;
}

void drgn_unitTrade(DRGN_Entity* self)
{
	DRGN_Entity* right;
	DRGN_Entity* left;
	DRGN_Entity* up;
	DRGN_Entity* down;
	DRGN_Unit* unit;
	DRGN_Player* player;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	drgn_unitMoveFree(self);
	drgn_entityFree(unit->menuCursor);
	unit->menuCursor = NULL;
	drgn_unitMenuFree(unit);

	right = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self);
	left = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self);
	down = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self);
	up = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self);
	player = (DRGN_Player*)self->curr->data;

	if (right)
	{
		self->curr->pos = right->pos;
		player->targets[player->totalTargets] = right;
		player->totalTargets++;
	}
	if (left)
	{
		self->curr->pos = left->pos;
		player->targets[player->totalTargets] = left;
		player->totalTargets++;
	}
	if (up)
	{
		self->curr->pos = up->pos;
		player->targets[player->totalTargets] = up;
		player->totalTargets++;
	}
	if (down)
	{
		self->curr->pos = down->pos;
		player->targets[player->totalTargets] = down;
		player->totalTargets++;
	}

	player->targeting = 1;
	self->curr->inactive = 0;
	player->pressed = 0;
	self->curr->curr = self;
}

void drgn_unitRescue(DRGN_Entity* self)
{
	DRGN_Entity* right;
	DRGN_Entity* left;
	DRGN_Entity* up;
	DRGN_Entity* down;
	DRGN_Unit* unit;
	DRGN_Player* player;

	if (!self || !self->data || !self->curr || !self->curr->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	drgn_unitMoveFree(self);
	drgn_entityFree(unit->menuCursor);
	unit->menuCursor = NULL;
	drgn_unitMenuFree(unit);

	right = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x + 64, self->pos.y), self);
	left = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x - 64, self->pos.y), self);
	down = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y + 64), self);
	up = drgn_entityGetSelectionByPosition(DRGN_BLUE, vector2d(self->pos.x, self->pos.y - 64), self);
	player = (DRGN_Player*)self->curr->data;

	if (right)
	{
		self->curr->pos = right->pos;
		player->targets[player->totalTargets] = right;
		player->totalTargets++;
	}
	if (left)
	{
		self->curr->pos = left->pos;
		player->targets[player->totalTargets] = left;
		player->totalTargets++;
	}
	if (up)
	{
		self->curr->pos = up->pos;
		player->targets[player->totalTargets] = up;
		player->totalTargets++;
	}
	if (down)
	{
		self->curr->pos = down->pos;
		player->targets[player->totalTargets] = down;
		player->totalTargets++;
	}

	player->targeting = 1;
	self->curr->inactive = 0;
	player->pressed = 0;
	self->curr->curr = self;
}

void drgn_unitMenuFree(DRGN_Unit* self)
{
	if (!self || !self->menuWindow)
	{
		return;
	}

	for (int bogus = 0; bogus < self->menuMax; bogus++)
	{
		if (!self->menuWindow[bogus])
		{
			continue;
		}

		drgn_windowFree(self->menuWindow[bogus]);
		self->menuWindow[bogus] = NULL;
	}

	self->menuMax = 0;
}

void drgn_unitSelectedMenuItem(DRGN_Entity* self)
{
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (unit->currentAction != DRGN_MOVE)
	{
		//slog("incorrect prerequisite action %i", unit->currentAction);
		return;
	}

	for (int bogus = 0; bogus < unit->menuMax; bogus++)
	{
		/*if (unit->menuWindow[bogus] && unit->menuWindow[bogus]->selected)
		{
			//unit->currentAction = drgn_windowMenuItemFromText(unit->menuWindow[bogus]);
			slog("got action");
			return;
		}*/
	}
}

void drgn_unitInteractionByEnum(DRGN_Entity* self, DRGN_Entity* other)
{
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		slog("no unit");
		return;
	}

	slog("pressed");
	unit = (DRGN_Unit*)self->data;

	switch (unit->currentAction)
	{
	case DRGN_TALK:

		if (!other)
		{
			slog("Other unit does not exist");
			return;
		}

		drgn_unitActionTalk(self, other);
		break;
	case DRGN_MELEE_ATTACK:
		
		if (!other)
		{
			return;
		}

		drgn_unitActionAttack(self, other);
		break;

	case DRGN_MAGIC_ATTACK:
		if (!other)
		{
			return;
		}

		//check if counter attack is possible here (ranged may not get countered some times)
		drgn_unitActionMagicAttack(self, other, 0);
		break;

	case DRGN_HEAL:

		if (!other)
		{
			return;
		}
		drgn_unitActionHeal(self, other);
		break;

	case DRGN_TRADE:

		if (!other)
		{
			return;
		}

		drgn_unitActionTrade(self, other);
		break;

	case DRGN_RESCUE:

		if (!other)
		{
			return;
		}

		drgn_unitActionRescue(self, other);
		break;
	case DRGN_ITEM_SHOP:
		//drgn_unitActionShop(self);
		break;
	default:
		break;
	}
}

void drgn_unitActionShop(DRGN_Entity* self)
{
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;
	drgn_selectorThink(unit->selector);
	drgn_selectorUpdate(unit->selector);
}

void drgn_unitActionTalk(DRGN_Entity* self, DRGN_Entity* other)
{
	other->affiliation = DRGN_BLUE;
	other->color = GFC_COLOR_BLUE;
	drgn_unitWait(self);
}

void drgn_unitActionAttack(DRGN_Entity* self, DRGN_Entity* other)
{
	DRGN_Unit* selfUnit;
	DRGN_Unit* otherUnit;
	int damage;

	if (!self || !self->data || !other || !other->data)
	{
		return;
	}

	selfUnit = (DRGN_Unit*)self->data;
	otherUnit = (DRGN_Unit*)other->data;

	if (selfUnit->stats[5] > otherUnit->stats[5])
	{
		damage = selfUnit->stats[2] + 5 - otherUnit->stats[7];

		if (damage < 0)
		{
			damage = 0;
		}

		otherUnit->currentHP -= damage;
		slog("Player dealt %i damage", damage);

		if (otherUnit->currentHP < 1)
		{
			drgn_entityFree(other);
			slog("Player killed an enemy");
			selfUnit->exp += 30;

			if (selfUnit->exp >= 100)
			{
				slog("Player leveled up!");
				//run level up function here
			}
			drgn_unitWait(self);
			return;
		}

		damage = otherUnit->stats[2] + 5 - selfUnit->stats[7];

		if (damage < 0)
		{
			damage = 0;
		}

		selfUnit->currentHP -= damage;
		slog("enemy dealt %i damage", damage);

		if (selfUnit->currentHP < 1)
		{
			drgn_unitWait(self);
			drgn_entityFree(self);
			slog("Enemy killed a player");
			return;
		}
	}
	else if (selfUnit->stats[5] < otherUnit->stats[5])
	{
		damage = otherUnit->stats[2] + 5 - selfUnit->stats[7];

		if (damage < 0)
		{
			damage = 0;
		}

		selfUnit->currentHP -= damage;
		slog("enemy dealt %i damage", damage);

		if (selfUnit->currentHP < 1)
		{
			drgn_unitWait(self);
			drgn_entityFree(self);
			slog("Enemy killed a player");
			return;
		}

		damage = selfUnit->stats[2] + 5 - otherUnit->stats[7];

		if (damage < 0)
		{
			damage = 0;
		}

		otherUnit->currentHP -= damage;
		slog("Player dealt %i damage", damage);

		if (otherUnit->currentHP < 1)
		{
			drgn_entityFree(other);
			slog("Player killed an enemy");
			selfUnit->exp += 30;

			if (selfUnit->exp >= 100)
			{
				slog("Player leveled up!");
				//run level up function here
			}
			drgn_unitWait(self);
			return;
		}
	}
	else
	{
		damage = otherUnit->stats[2] + 5 - selfUnit->stats[7];

		if (damage < 0)
		{
			damage = 0;
		}

		selfUnit->currentHP -= damage;
		slog("enemy dealt %i damage", damage);

		damage = selfUnit->stats[2] + 5 - otherUnit->stats[7];

		if (damage < 0)
		{
			damage = 0;
		}

		otherUnit->currentHP -= damage;
		slog("Player dealt %i damage", damage);

		if (otherUnit->currentHP < 1)
		{
			drgn_entityFree(other);
			slog("Player killed an enemy");
			selfUnit->exp += 30;

			if (selfUnit->exp >= 100)
			{
				slog("Player leveled up!");
				//run level up function here
			}
			drgn_unitWait(self);
			return;
		}

		if (selfUnit->currentHP < 1)
		{
			drgn_unitWait(self);
			drgn_entityFree(self);
			slog("Enemy killed a player");
			return;
		}
	}

	drgn_unitWait(self);
}

void drgn_unitActionMagicAttack(DRGN_Entity* self, DRGN_Entity* other, Uint8 counter)
{
	DRGN_Unit* selfUnit;
	DRGN_Unit* otherUnit;
	DRGN_InventoryItem* selfItem;
	DRGN_InventoryItem* otherItem;
	int damage;

	if (!self || !self->data || !other || !other->data)
	{
		return;
	}

	selfUnit = (DRGN_Unit*)self->data;
	otherUnit = (DRGN_Unit*)other->data;
	selfItem = drgn_inventoryCheckItemTypeInInventory(selfUnit->inventory, DRGN_ARCANE);
	otherItem = drgn_inventoryCheckItemTypeInInventory(otherUnit->inventory, DRGN_ARCANE);

	if (selfItem && !otherItem)
	{
		damage = selfUnit->stats[3] + 5 - otherUnit->stats[8];

		if (damage < 0)
		{
			damage = 0;
		}

		otherUnit->currentHP -= damage;
		slog("Player dealt %i damage", damage);

		if (otherUnit->currentHP < 1)
		{
			drgn_entityFree(other);
			slog("Player killed an enemy");
			selfUnit->exp += 30;

			if (selfUnit->exp >= 100)
			{
				slog("Player leveled up!");
				//run level up function here
			}
			drgn_unitWait(self);
			return;
		}
	}
	else if (!selfItem && otherItem)
	{
		damage = otherUnit->stats[3] + 5 - selfUnit->stats[8];

		if (damage < 0)
		{
			damage = 0;
		}

		selfUnit->currentHP -= damage;
		slog("enemy dealt %i damage", damage);

		if (selfUnit->currentHP < 1)
		{
			drgn_unitWait(self);
			drgn_entityFree(self);
			slog("Enemy killed a player");
			return;
		}
	}
	else if (selfItem && otherItem)
	{
		if (selfUnit->stats[5] > otherUnit->stats[5])
		{
			damage = selfUnit->stats[3] + 5 - otherUnit->stats[8];

			if (damage < 0)
			{
				damage = 0;
			}

			otherUnit->currentHP -= damage;
			slog("Player dealt %i damage", damage);

			if (otherUnit->currentHP < 1)
			{
				drgn_entityFree(other);
				slog("Player killed an enemy");
				selfUnit->exp += 30;

				if (selfUnit->exp >= 100)
				{
					slog("Player leveled up!");
					//run level up function here
				}
				drgn_unitWait(self);
				return;
			}

			damage = otherUnit->stats[3] + 5 - selfUnit->stats[8];

			if (damage < 0)
			{
				damage = 0;
			}

			selfUnit->currentHP -= damage;
			slog("enemy dealt %i damage", damage);

			if (selfUnit->currentHP < 1)
			{
				drgn_unitWait(self);
				drgn_entityFree(self);
				slog("Enemy killed a player");
				return;
			}
		}
		else if (selfUnit->stats[5] < otherUnit->stats[5])
		{
			damage = otherUnit->stats[3] + 5 - selfUnit->stats[8];

			if (damage < 0)
			{
				damage = 0;
			}

			selfUnit->currentHP -= damage;
			slog("enemy dealt %i damage", damage);

			if (selfUnit->currentHP < 1)
			{
				drgn_unitWait(self);
				drgn_entityFree(self);
				slog("Enemy killed a player");
				return;
			}

			damage = selfUnit->stats[3] + 5 - otherUnit->stats[8];

			if (damage < 0)
			{
				damage = 0;
			}

			otherUnit->currentHP -= damage;
			slog("Player dealt %i damage", damage);

			if (otherUnit->currentHP < 1)
			{
				drgn_entityFree(other);
				slog("Player killed an enemy");
				selfUnit->exp += 30;

				if (selfUnit->exp >= 100)
				{
					slog("Player leveled up!");
					//run level up function here
				}
				drgn_unitWait(self);
				return;
			}
		}
		else
		{
			damage = otherUnit->stats[3] + 5 - selfUnit->stats[8];

			if (damage < 0)
			{
				damage = 0;
			}

			selfUnit->currentHP -= damage;
			slog("enemy dealt %i damage", damage);

			damage = selfUnit->stats[3] + 5 - otherUnit->stats[8];

			if (damage < 0)
			{
				damage = 0;
			}

			otherUnit->currentHP -= damage;
			slog("Player dealt %i damage", damage);

			if (otherUnit->currentHP < 1)
			{
				drgn_entityFree(other);
				slog("Player killed an enemy");
				selfUnit->exp += 30;

				if (selfUnit->exp >= 100)
				{
					slog("Player leveled up!");
					//run level up function here
				}
				drgn_unitWait(self);
				return;
			}

			if (selfUnit->currentHP < 1)
			{
				drgn_unitWait(self);
				drgn_entityFree(self);
				slog("Enemy killed a player");
				return;
			}
		}
	}

	drgn_unitWait(self);
}

void drgn_unitActionHeal(DRGN_Entity* self, DRGN_Entity* other)
{
	DRGN_Unit* selfUnit;
	DRGN_Unit* otherUnit;
	int healing;

	if (!self || !self->data || !other || !other->data)
	{
		return;
	}

	selfUnit = (DRGN_Unit*)self->data;
	otherUnit = (DRGN_Unit*)other->data;

	if (!drgn_inventoryCheckItemTypeInInventory(selfUnit->inventory, DRGN_DIVINE))
	{
		return;
	}

	if (otherUnit->currentHP >= otherUnit->stats[1])
	{
		return;
	}

	healing = selfUnit->stats[3] + selfUnit->stats[8];
	otherUnit->currentHP += healing;
	slog("healed for %i health", healing);

	if (otherUnit->currentHP > otherUnit->stats[1])
	{
		otherUnit->currentHP = otherUnit->stats[1];
	}

	drgn_unitWait(self);
}

void drgn_unitActionTrade(DRGN_Entity* self, DRGN_Entity* other)
{
	DRGN_Unit* selfUnit;
	DRGN_Unit* otherUnit;
	DRGN_InventoryItem selfItem;
	DRGN_InventoryItem otherItem;

	if (!self || !self->data || !other || !other->data)
	{
		return;
	}

	selfUnit = (DRGN_Unit*)self->data;
	otherUnit = (DRGN_Unit*)other->data;
	selfItem = selfUnit->inventory->itemList[selfUnit->inventory->equipped];
	otherItem = otherUnit->inventory->itemList[otherUnit->inventory->equipped];
	selfUnit->inventory->itemList[selfUnit->inventory->equipped] = otherItem;
	otherUnit->inventory->itemList[otherUnit->inventory->equipped] = selfItem;
	drgn_unitWait(self);
}

void drgn_unitActionRescue(DRGN_Entity* self, DRGN_Entity* other)
{
	DRGN_Unit* selfUnit;
	DRGN_Unit* otherUnit;

	if (!self || !self->data || !other)
	{
		return;
	}

	selfUnit = (DRGN_Unit*)self->data;
	otherUnit = (DRGN_Unit*)other->data;
	selfUnit->rescuedUnit = other;
	other->pos.x = -1;
	other->pos.y = 1;
	otherUnit->rescued = 1;
	other->inactive = 1;
	drgn_unitWait(self);
}

void drgn_unitItemShop(DRGN_Entity* self)
{
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (!unit->shop)
	{
		unit->shop = drgn_shopCreate("testShop", self);
		drgn_windowChangePosition(unit->shop, vector2d(self->pos.x + 96, self->pos.y));
		drgn_unitMoveFree(self);
		drgn_entityFree(unit->menuCursor);
		unit->menuCursor = NULL;
		drgn_unitMenuFree(unit);
		unit->selector = drgn_selectorNew(unit->shop);
	}
}

void drgn_unitBuy(DRGN_Entity* self)
{
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (!unit->shop || !unit->shop->elementsNum)
	{
		return;
	}

	for (int bogus = 0; bogus < unit->shop->elementsNum; bogus++)
	{
		if (!unit->shop->elements[bogus])
		{
			continue;
		}

		if (gfc_strlcmp(unit->shop->elements[bogus]->name, "buyButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 0;
			slog("made unselectable");
		}
		else if (gfc_strlcmp(unit->shop->elements[bogus]->name, "sellButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 0;
			slog("made unselectable");
		}
		else if (gfc_strlcmp(unit->shop->elements[bogus]->name, "buyItemButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 1;
			slog("made selectable");
		}
		else if (gfc_strlcmp(unit->shop->elements[bogus]->name, "sellItemButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 0;
		}
	}

	drgn_selectorFindNewSelection(unit->selector);
}

void drgn_unitSell(DRGN_Entity* self)
{
	DRGN_Unit* unit;

	if (!self || !self->data)
	{
		return;
	}

	unit = (DRGN_Unit*)self->data;

	if (!unit->shop || !unit->shop->elementsNum)
	{
		return;
	}

	for (int bogus = 0; bogus < unit->shop->elementsNum; bogus++)
	{
		if (!unit->shop->elements[bogus])
		{
			continue;
		}

		if (gfc_strlcmp(unit->shop->elements[bogus]->name, "buyButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 0;
			slog("made unselectable");
		}
		else if (gfc_strlcmp(unit->shop->elements[bogus]->name, "sellButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 0;
			slog("made unselectable");
		}
		else if (gfc_strlcmp(unit->shop->elements[bogus]->name, "buyItemButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 0;
		}
		else if (gfc_strlcmp(unit->shop->elements[bogus]->name, "sellItemButton") == 0)
		{
			unit->shop->elements[bogus]->selectable = 1;
		}
	}

	drgn_selectorFindNewSelection(unit->selector);
}