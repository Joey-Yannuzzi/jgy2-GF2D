#ifndef __DRGN_PLAYER_H__
#define __DRGN_PLAYER_H__

#include "drgn_entity.h"

typedef struct
{
	//int test;
	int pressed; //to check if spacebar is pressed this frame
	DRGN_Entity* terrainWindow; //terrain window to display terrain being hovered over
	DRGN_Entity* unitWindow; //window that display's unit's name/class
	DRGN_Entity* unitWindowSource; //entity where the unitWindow is getting information from
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