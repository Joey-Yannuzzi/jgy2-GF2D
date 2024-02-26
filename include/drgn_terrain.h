#ifndef __DRGN_TERRAIN_H__
#define __DRGN_TERRAIN_H__

#include "simple_json.h"
#include "drgn_entity.h"

static SJson* _terrainJson = NULL;
static SJson* _terrainList = NULL;

typedef struct
{
	const char* name; //name to be displayed on the UI
	int tile; //tile of the terrain on the tile list
	float moveValue; //amount of move points to be taken away from movement pool when passing through this space
	int avoidBonus; //extra avoid given by being in this terrain; optional
	int defBonus; //extra defense given by being in this terrain; optional
	const char** ignoreClass; //list of names of classes to not let pass through this terrain; optional; needs to be freed if it exists
}
DRGN_Terrain;

void drgn_terrainThink(DRGN_Entity* self);

void drgn_terrainUpdate(DRGN_Entity* self);

void drgn_terrainFree(DRGN_Entity* self);

/*
* @brief creates a new terrain entity to be interacted with
* @param tile the tile that corresponds with this entity type
* @param pos the postion of the terrain in world space
* @return pointer to the entity created
*/
DRGN_Entity* drgn_terrainNew(int tile, Vector2D pos);

/*
* @brief initiates the terrain file specified
* @param name the name of the file
*/
void drgn_terrainFileInit(const char* name);

/*
* @brief frees the terrain file
*/
void drgn_terrainFileFree();

/*
* @brief grabs a terrain by its tile index value
* @param index the number to search the unit by
* @return the Json object containing the unit in question
* @note unless the terrain json is edited in a way that causes items to be out of order, each tile index should be in ascending order
*/
SJson* drgn_terrainGetDefByNumber(int index);

#endif
