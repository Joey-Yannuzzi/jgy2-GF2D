#ifndef __DRGN_ENTITY_H__
#define __DRGN_ENTITY_H__

#include "gfc_types.h"
#include "gf2d_sprite.h"
/*
* @purpose default entity skeleton; all entities should inherit from this
*/

typedef struct DRGN_Entity_S
{
	Uint8 _inuse; //used to check for memory usage
	Sprite* sprite; //entity sprite pointer; NULL if none exists
	float frame; //current animation frame of the entity's sprite
	Vector2D pos; //position of the entity in space
	Vector2D velocity; //velocity vector for an entity
	void* data; //for any additional data default entity may not have; NULL if no such data exists

	void (*think) (struct DRGN_Entity_S* self); //call function to make decisions
	void (*update) (struct DRGN_Entity_S* self); //call function to execute decisions from think function
	void (*free) (struct DRGN_Entity_S* self); //call function to clean up custom allocated data
}
DRGN_Entity;

/*
* @brief initializes the entity management system; queues cleanup on exit
* @param max maximum number of entities at a given time
*/
void drgn_entitySystemInit(Uint32 max);

void drgn_entitySystemClose();
/*
* @brief cleans up all active entities from memory
* @param ignore entities that do not get cleaned up
*/
void drgn_entityCleanAll(DRGN_Entity* ignore);

/*
* @brief obtain an entity for use
* @return a blank entity if one is available; NULL if max is reached
*/
DRGN_Entity* drgn_entityNew();

/*
* @brief clean up an entity for future use
* @param self the entity to be cleaned
*/
void drgn_entityFree(DRGN_Entity* self);

/*
* @brief run think function for all active entities
*/
void drgn_entitySystemThink();

/*
* @brief run update functions for all active entities
*/
void drgn_entitySystemUpdate();

/*
* @brief draw all active entities
*/
void drgn_entitySystemDraw();

#endif