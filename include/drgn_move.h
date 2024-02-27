#ifndef __DRGN_MOVE_H__
#define __DRGN_MOVE_H__

#include "drgn_entity.h"

typedef struct
{
	DRGN_Entity* moveList; //pointer to list of all movement squares active
	Uint32 max; //maximum number of movement squares
}
DRGN_MovementManager;

static DRGN_MovementManager _moveManager = { 0 };

void drgn_moveThink();

void drgn_moveUpdate();

void drgn_moveFree();

DRGN_Entity* drgn_moveNew(Color color, Vector2D pos, const char* sprite, int width, int height);

void drgn_moveManagerInit(Uint32 max);

void drgn_moveManagerClose();

DRGN_Entity* drgn_moveGetDuplicate(DRGN_Entity* self);

#endif
