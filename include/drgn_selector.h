#ifndef __DRGN_SELECTOR_H__
#define __DRGN_SELECTOR_H__

#include "drgn_window.h"

typedef struct
{
	DRGN_Window* parent; //the window to be selected from, DO NOT FREE THIS
	int dir; //the positive or negative direction the index moved in this frame
	int current; //index of the currently selected windel
}
DRGN_Selector;

DRGN_Selector* drgn_selectorNew(DRGN_Window* window);

void drgn_selectorThink(DRGN_Selector* selector);

void drgn_selectorUpdate(DRGN_Selector* selector);

void drgn_selectorFree(DRGN_Selector* selector);

#endif
