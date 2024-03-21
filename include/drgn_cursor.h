#ifndef __DRGN_CURSOR_H__
#define __DRGN_CURSOR_H__

#include "drgn_entity.h"

typedef struct
{
	Vector2D upperBounds;
	Vector2D lowerBounds;
	int frameskip;
	Uint8 pressed;
}
DRGN_Cursor;

DRGN_Entity* drgn_cursorNew(Vector2D pos, Vector2D upperBounds, Vector2D lowerBounds);

void drgn_cursorUpdate(DRGN_Entity* self);

void drgn_cursorThink(DRGN_Entity* self);

void drgn_cursorFree(DRGN_Entity* self);
#endif