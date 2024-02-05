#ifndef __DRGN_ARMY_H__
#define __DRGN_ARMY_H__

#include "drgn_entity.h"

typedef struct
{
	int totalUnits; //the total army count of an army
	int activeUnits; //the number of units who have not taken their turn yet
	DRGN_Entity* unitList; //pointer to valid units for this army
	char* name; //army's name
}
DRGN_Army;

void drgn_armyThink(DRGN_Entity* self);

void drgn_armyUpdate(DRGN_Entity* self);

void drgn_armyFree(DRGN_Entity* self);

/*
* @brief creates an army entity
* @return pointer to the army entity; NULL if army could not be made
*/
DRGN_Entity* drgn_armyNew(enum DRGN_Affiliation affiliation, char* name);

#endif