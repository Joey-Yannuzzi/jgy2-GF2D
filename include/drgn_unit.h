#ifndef __DRGN_UNIT_H__
#define __DRGN_UNIT_H__

#include "drgn_entity.h"
#include "drgn_inventory.h"

enum DRGN_UnitWeaponLvl
{
	DRGN_NOT_WIELDABLE,
	DRGN_E,
	DRGN_D,
	DRGN_C,
	DRGN_B,
	DRGN_A,
	DRGN_S
};

typedef struct
{
	int stats[11]; //unit stats; 0 lvl, 1 hp, 2, str, 3 mag, 4 skl, 5 spd, 6 lck, 7 def, 8 res, 9 mov, 10 bld
	DRGN_Inventory* inventory; //list of unit's inventory items
	Sprite* moveTile; //pointer to movement tile sprite
	Sprite* attackTitle; //pointer to attack tile sprite; NULL if cannot attack
	Sprite* animationSprite; //pointer to closeup animation spritesheet;
	Uint8 animate; //true if animations are on for this unit, false if not; defaults to true
	const char* name; //the units name or rank if generic
	enum DRGN_UnitWeaponLvl weaponLvls[9]; //Counters for weapon levels; 0 or NULL if unit cannot wield that weapon; 0 sword, 1 lance, 2, axe, 3 bow, 4 rapier, 5 dagger, 6 arcane, 7 divine, 8 natural
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
DRGN_Entity* drgn_unitNew(int* stats, size_t statSize, const char* inventory[], const char* name, enum DRGN_Affiliation affiliation);

#endif