#ifndef __DRGN_UNIT_H__
#define __DRGN_UNIT_H__

#include "drgn_entity.h"

typedef struct
{
	int lvl, hp, pow, skl, spd, lck, def, res, mov, morale; //unit stats
	DRGN_Entity* inventory; //list of unit's inventory items
	Sprite* moveTile; //pointer to movement tile sprite
	Sprite* attackTitle; //pointer to attack tile sprite; NULL if cannot attack
	Sprite* animationSprite; //pointer to closeup animation spritesheet;
	bool animate; //true if animations are on for this unit, false if not; defaults to true
	char* name; //the units name or rank if generic
}
DRGN_Unit;

#endif