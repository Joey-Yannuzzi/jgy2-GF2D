#ifndef __DRGN_WINDOW_H__
#define __DRGN_WINDOW_H__

#include "gfc_list.h"
#include "drgn_entity.h"
#include "drgn_font.h"
#include "drgn_unit.h"

/*
* @purpose window struct and functions to create UI windows
*/

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

#endif
