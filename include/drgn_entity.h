#ifndef __DRGN_ENTITY_H__
#define __DRGN_ENTITY_H__

#include "gfc_types.h"
#include "gf2d_sprite.h"
#include "drgn_window.h"

/*
* @purpose default entity skeleton; all entities should inherit from this
*/

enum DRGN_Affiliation
{
	DRGN_DEFAULT, //only terrain will use this value
	DRGN_BLUE,
	DRGN_RED,
	DRGN_GREEN,
	DRGN_CURSOR,
	DRGN_TILE,
	DRGN_UI
};

typedef struct DRGN_Entity_S
{
	Uint8 _inuse; //used to check for memory usage
	Sprite* sprite; //entity sprite pointer; NULL if none exists
	float frame; //current animation frame of the entity's sprite
	Vector2D pos; //position of the entity in space
	Vector2D velocity; //velocity vector for an entity
	Vector2D scale; //scale entity should be scaled up to; NULL if no scaling is needed
	Color color; //main color of the entity; NULL if not used
	enum DRGN_Affiliation affiliation; //the units army affiliation; should never be set to DEFAULT and should be checked/caught
	Uint8 selected; //used to check if this entity is selected
	struct DRGN_Entity_S* curr; //currently selected entity
	Uint8 colorSet; //value to check if a color is set
	Uint8 offset; //value to check if offset should be used when drawing; only used for UI
	Vector2D offsetVal; //value of the offset; only used for UI
	Uint8 inactive; //set this off when entity is no longer active, but does not want to be destroyed yet
	Uint8 win; //checks if this unit has completed the objective
	DRGN_Windel** pauseWindels; //windels for the pause menu; only for use in pause menu
	void* data; //for any additional data default entity may not have; NULL if no such data exists

	void (*think) (struct DRGN_Entity_S* self); //call function to make decisions
	void (*update) (struct DRGN_Entity_S* self); //call function to execute decisions from think function
	void (*free) (struct DRGN_Entity_S* self); //call function to clean up custom allocated data
	void (*draw) (struct DRGN_Entity_S* self); //call function to draw custom assets for an entity
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

/*
* @brief writes all entities with certain affiliation to entity list pointer
* @param list the entity pointer that is written to
* @param affiliation the desired affiliation
*/
DRGN_Entity* drgn_entityGetUnitsByAffiliation(enum DRGN_Affiliation affiliation);

DRGN_Entity* drgn_entityGetSelectionByPosition(enum DRGN_Affiliation affiliation, Vector2D pos, DRGN_Entity* self);

void drgn_entitySetAllInactive();

#endif