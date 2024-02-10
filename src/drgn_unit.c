#include "simple_logger.h"
#include "drgn_unit.h"

DRGN_Entity* drgn_unitNew(int* stats, DRGN_Entity* inventory, char* name, enum DRGN_Affiliation affiliation)
{
	DRGN_Entity* self;
	DRGN_Unit temp;
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

	temp.lvl = stats[0];
	temp.hp = stats[1];
	temp.pow = stats[2];
	temp.skl = stats[3];
	temp.spd = stats[4];
	temp.lck = stats[5];
	temp.def = stats[6];
	temp.res = stats[7];
	temp.mov = stats[8];
	temp.morale = stats[9];
	temp.inventory = inventory;
	temp.moveTile = NULL; //same regardless of unit
	temp.attackTitle = NULL; //same as above
	temp.animate = 1;
	temp.name = name;
	self->data = &temp;
	slog("%s spawned", name);
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

}