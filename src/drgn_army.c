#include "simple_logger.h"
#include "drgn_army.h"

DRGN_Entity* drgn_armyNew(enum DRGN_Affiliation affiliation, char* name)
{
	DRGN_Entity* self;
	DRGN_Army temp;
	DRGN_Entity* list;
	self = drgn_entityNew();

	if (!self)
	{
		slog("An army cannot be created");
		return NULL;
	}

	list = drgn_entityGetUnitsByAffiliation(affiliation);

	if (!list)
	{
		slog("No units in your army");
		drgn_entityFree(self);
		return NULL;
	}
	
	self->affiliation = affiliation;
	self->think = drgn_armyThink;
	self->update = drgn_armyUpdate;
	self->free = drgn_armyFree;
	temp.totalUnits = 5;
	temp.activeUnits = temp.totalUnits;
	temp.unitList = list;
	temp.name = name;
	self->data = &temp;

	/*switch (self->affiliation)
	{
	case DRGN_DEFAULT:
		slog("Cannot create invalid army");
		return NULL;
	case DRGN_BLUE:
		slog("Blue army created");
		break;
	case DRGN_RED:
		slog("Red army created");
		break;
	case DRGN_GREEN:
		slog("Green army created");
		break;
	}*/

	slog("%s army created", name);
	return (self);
}

void drgn_armyThink(DRGN_Entity* self)
{
	if (!self)
	{
		slog("no army to think");
		return;
	}
}

void drgn_armyUpdate(DRGN_Entity* self)
{
	if (!self)
	{
		slog("no army to update");
		return;
	}
}

void drgn_armyFree(DRGN_Entity* self)
{
	if (!self)
	{
		slog("no army to free");
		return;
	}
}