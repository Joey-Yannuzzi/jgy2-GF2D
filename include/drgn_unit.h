#ifndef __DRGN_UNIT_H__
#define __DRGN_UNIT_H__

#include "drgn_entity.h"

typedef struct
{
	int lvl, hp, str, mgc, skl, spd, lck, def, res, mov, morale; //unit stats
	DRGN_Entity* inventory; //list of unit's inventory items
	Sprite* moveTile; //pointer to movement tile sprite
	Sprite* attackTitle; //pointer to attack tile sprite; NULL if cannot attack
	Sprite* animationSprite; //pointer to closeup animation spritesheet;
	Uint8 animate; //true if animations are on for this unit, false if not; defaults to true
	char* name; //the units name or rank if generic
	void* data; //used for class specific attributes
}
DRGN_Unit;

void drgn_unitThink(DRGN_Entity* self);

void drgn_unitUpdate(DRGN_Entity* self);

void drgn_unitFree(DRGN_Entity* self);

/*
* @brief used to create a generic unit entity; should only be called in class specific c files
* @param stats pointer to the stats list given; should never be NULL
* @param inventory entity pointer of the unit's current inventory
* @param name string representing the unit's name
* @param affiliation the affiliation for the unit
* @return pointer to an entity; NULL if it could not be created
*/
DRGN_Entity* drgn_unitNew(int* stats, DRGN_Entity* inventory, char* name, enum DRGN_Affiliation affiliation);

#endif