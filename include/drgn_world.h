#ifndef __DRGN_WORLD_H__
#define __DRGN_WORLD_H__

#include "gf2d_sprite.h"

typedef struct
{
	Sprite* background; //backround sprite of world/level; unsure if needed at this moment
	Sprite* tileSet; //the sprite map for the tiles
	Uint8* tileMap; //the world's tilemap
	Uint32 height; //height of map
	Uint32 width; //witdth of map
	Sprite* tileLayer; //prerendered tiles
}
DRGN_World;

/*
* @brief creates new world
* @param width the tile's width
* @return the alloacated world pointer; NULL if none could be created or there is an error
*/
DRGN_World* drgn_worldNew(int width, int height);

/*
* @brief frees up the allocated memory the world was using for future use
* @param self a world pointer of the world we want to destroy
*/
void drgn_worldFree(DRGN_World* self);

/*
* @brief draws the world
* @param self the world to be drawn
*/
void drgn_worldDraw(DRGN_World* self);

DRGN_World* drgn_worldNewTest();

void drgn_worldTileLayerRender(DRGN_World* self);

#endif
