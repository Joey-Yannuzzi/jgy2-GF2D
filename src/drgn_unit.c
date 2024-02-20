#include "simple_logger.h"
#include "drgn_unit.h"

DRGN_Entity* drgn_unitNew(int* stats, size_t statSize, const char* inventory[], const char* name, enum DRGN_Affiliation affiliation)
{
	DRGN_Entity* self;
	DRGN_Unit* temp;
	//const char* names[] = { "smallPotion", "lvlIncrease", "mediumPotion", "largePotion", "smallPotion"};
	self = drgn_entityNew();

	if (!self)
	{
		slog("No free space for unit exists");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all("images/newUnit.png", 64, 64, 4, 0);
	self->frame = 0;
	self->pos = vector2d(0, 0);
	self->scale = vector2d(1, 1);
	
	switch (affiliation)
	{
	case DRGN_DEFAULT:
		slog("Cannot create invalid army");
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
	}

	self->affiliation = affiliation;
	self->think = drgn_unitThink;
	self->update = drgn_unitUpdate;
	self->free = drgn_unitFree;

	if (!stats)
	{
		slog("No stats to populate unit");
		drgn_entityFree(self);
		return NULL;
	}

	temp = gfc_allocate_array(sizeof(DRGN_Unit), 1);

	if (!temp)
	{
		slog("unit data could not be allocated");
		return NULL;
	}

	self->data = temp;

	for (int bogus = 0; bogus < statSize; bogus++)
	{
		temp->stats[bogus] = stats[bogus];
	}

	slog("stats allocated");
	temp->moveTile = NULL; //same regardless of unit
	temp->attackTitle = NULL; //same as above
	temp->animate = 1;
	temp->name = name;
	temp->inventory = drgn_inventoryNew(inventory, 5);

	if (!temp->inventory)
	{
		slog("No inventory created");
		drgn_entityFree(self);
		return NULL;
	}

	slog("%s spawned", name);
	for (int bogus = 0; bogus < temp->inventory->curr; bogus++)
	{
		slog("Spawned with %s in inventory", &temp->inventory->itemList[bogus].name);
	}
	
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