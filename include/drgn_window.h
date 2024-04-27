#ifndef __DRGN_WINDOW_H__
#define __DRGN_WINDOW_H__

#include "gfc_list.h"
#include "drgn_entity.h"
#include "drgn_font.h"
#include "drgn_unit.h"
#include "drgn_windels.h"

/*
* @purpose window struct and functions to create UI windows

typedef struct
{
	char* texts; //all the texts for a certain window
	//List* children; //all children objects for the window; can include other entities or even other windows
}
DRGN_Window;

DRGN_Entity* drgn_windowNew(char* texts, const char* sprite, Uint32 width, Uint32 height, Vector2D pos, DRGN_Entity* curr);

void drgn_windowFree(DRGN_Entity* self);

void drgn_windowThink(DRGN_Entity* self);

void drgn_windowUpdate(DRGN_Entity* self);

void drgn_windowDraw(DRGN_Entity* self);

DRGN_Action drgn_windowMenuItemFromText(DRGN_Entity* self);
*/


/*
* @purpose a window system that supports different window elements, to be used to create UI elements and command windows dynamically as needed
*/

typedef struct
{
	Uint8 _inuse; //checks if space is being used by window currently
	Vector2D pos; //position of the window relative to world space or screen space
	Vector2D scale; //the scale of the window; set to 1 if no scaling should be applied
	Uint8 offsetPos; //flag for whether camera  offset should affect position; 0 if no offset should be applied
	DRGN_Windel** elements; //window elements that make up the window; must be freed
	Uint32 elementsNum; //number of elements in this window
}
DRGN_Window;

/*
* @brief create a new window from a json object with a specified name
* @param name the name if the window
* @return the window created
*/
DRGN_Window* drgn_windowNew(const char* name);

/*
* @brief free a window
* @param self the window to be freed
*/
void drgn_windowFree(DRGN_Window* self);

/*
* @brief update the window
* @param self the window
*/
void drgn_windowUpdate(DRGN_Window* self);

/*
* @brief draw the window
* @param self the window
*/
void drgn_windowDraw(DRGN_Window* self);

void drgn_windowChangePosition(DRGN_Window* self, Vector2D changePos);

void drgn_windowDrawAll();

void drgn_windowUpdateAll();

void drgn_windowFreeAll();

void drgn_windowManagerNew(Uint32 max);

void drgn_windowFileInit(const char* name);

#endif
