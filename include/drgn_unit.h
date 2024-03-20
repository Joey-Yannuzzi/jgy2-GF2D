#ifndef __DRGN_UNIT_H__
#define __DRGN_UNIT_H__

#include "drgn_entity.h"
#include "drgn_inventory.h"
#include "drgn_move.h"

#define _drgn_weapons 9 //0 sword, 1 lance, 2 axe, 3 bow, 4 rapier, 5 dagger, 6 arcane, 7 divine, 8 natural
#define _drgn_stats 11 //0 lvl, 1 hp, 2, str, 3 mag, 4 skl, 5 spd, 6 lck, 7 def, 8 res, 9 mov, 10 bld

static SJson* _unitJson = NULL;
static SJson* _unitIds = NULL;

enum DRGN_UnitWeaponLvl
{
	DRGN_WEAPON_NONE,
	DRGN_WEAPON_E,
	DRGN_WEAPON_D,
	DRGN_WEAPON_C,
	DRGN_WEAPON_B,
	DRGN_WEAPON_A,
	DRGN_WEAPON_S
};

typedef struct
{
	int stats[_drgn_stats]; //unit stats
	DRGN_Inventory* inventory; //list of unit's inventory items
	Sprite* moveTile; //pointer to movement tile sprite
	Sprite* attackTitle; //pointer to attack tile sprite; NULL if cannot attack
	Sprite* animationSprite; //pointer to closeup animation spritesheet;
	Uint8 animate; //true if animations are on for this unit, false if not; defaults to true
	const char* name; //the units name or rank if generic
	enum DRGN_UnitWeaponLvl weaponLvls[_drgn_weapons]; //Counters for weapon levels; 0 or NULL if unit cannot wield that weapon
	int exp; //unit experience; goes from 0 to 99 at 100 unit levels up; defaults as 0
	int weaponExp[_drgn_weapons]; //experience for a units weapons; goes from 0 to 99 at 100 weapon level goes up one rank; defaults as 0
	int growths[_drgn_stats]; //unit growths
	int moveDrawn; //1 if movement is drawn, 0 if not
	Uint8* moveMap; //the units move map
	DRGN_Entity** moveTiles; //list of all units move tiles
	int moveTotal; //size of array to be created for move purposes
	int active; //check for if unit already acted this turn
	const char* class; //name of the class of unit
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
//DRGN_Entity* drgn_unitNew(int* stats, size_t statSize, const char* inventory[], const char* name, enum DRGN_Affiliation affiliation);
DRGN_Entity* drgn_unitNew(const char* name, const char* inventory[], enum DRGN_Affiliation affiliation, Vector2D pos);

/*
* @brief initializes def file that holds units
* @param name the name of the file
*/
void drgn_unitFileInit(const char* name);

void drgn_unitFileFree();

/*
* @brief gets the definition of a unit
* @param name the name of the unit
* @return the pointer to the json object of the unit
*/
SJson* drgn_unitGetDefByName(const char* name);

DRGN_Entity* drgn_unitMoveNew(DRGN_Entity* self, Vector2D pos, int index);

void drgn_unitCalcMove(DRGN_Entity* self, float move, Vector2D pos, int index);

void drgn_unitMoveFree(DRGN_Entity* self);

#endif