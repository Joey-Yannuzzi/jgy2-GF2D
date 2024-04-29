#ifndef __DRGN_PLAYER_H__
#define __DRGN_PLAYER_H__

#include "drgn_entity.h"
#include "drgn_window.h"

typedef struct
{
	//int test;
	int pressed; //to check if spacebar is pressed this frame
	DRGN_Window* terrainWindow; //terrain window to display terrain being hovered over
	DRGN_Window* statScreen; //the currently displayed stat screen; null if no stat screen is being shown
	DRGN_Entity* unitWindow; //window that display's unit's name/class
	DRGN_Entity* unitWindowSource; //entity where the unitWindow is getting information from
	DRGN_Entity** targets; //pointers to all the targets a unit can interact with; max 12
	Uint8 targeting; //used to determine if the player is moving normally or is targetting
	int totalTargets; //number of total targets currently
	int currentTarget; //index of the current target
	Uint8 statScreenPress; //determines if the button to show the stat screen has been pressed this frame
}
DRGN_Player;

void drgn_playerThink(DRGN_Entity* self);

void drgn_playerUpdate(DRGN_Entity* self);

void drgn_playerFree(DRGN_Entity* self);

/*
* @brief creates a player entity
* @return pointer to the player entity; NULL if player could not be made
* @param army an Entity pointer that points to the player controlled army
*/
DRGN_Entity* drgn_playerNew();

#endif