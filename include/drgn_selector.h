#ifndef __DRGN_SELECTOR_H__
#define __DRGN_SELECTOR_H__

#include "drgn_window.h"

typedef struct
{
	DRGN_Window* parent; //the window to be selected from, DO NOT FREE THIS
	DRGN_Windel** buttons; //list of all selectable buttons
	DRGN_Windel** texts; //the texts that match the buttons
	int dir; //the positive or negative direction the index moved in this frame
	int current; //index of the currently selected windel
	int max; //max number of windel elements
	int frameskip; //how many frames the selector has been running for
	int pressed; //true if the button was pressed this frame
}
DRGN_Selector;

DRGN_Selector* drgn_selectorNew(DRGN_Window* window);

void drgn_selectorThink(DRGN_Selector* selector);

void drgn_selectorUpdate(DRGN_Selector* selector);

void drgn_selectorFree(DRGN_Selector* selector);

void drgn_selectorFindNewSelection(DRGN_Selector* selector);

#endif
