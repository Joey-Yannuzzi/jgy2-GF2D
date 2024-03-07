#ifndef __DRGN_WINDOW_H__
#define __DRGN_WINDOW_H__

#include "gfc_list.h"
#include "gf2d_sprite.h"

/*
* @purpose Window system for UI purposes
*/

typedef struct
{
	Sprite* window; //the sprite for the window container
	List* data; //a list of the data for this window container
	Color color; //color of the window container
	Vector2D pos; //postion of the conmtainer in screen space
	Uint8 _inuse; //used to check for memory usage
}
DRGN_Window;

typedef struct
{
	DRGN_Window* windowList; //pointer list to all current windows
	Uint32 max; //maximum number of windows at one time
}
DRGN_WindowManager;

static DRGN_WindowManager _windowManager = { 0 }; //local global window manager

/*
* @brief create a new window container
* @param data a list pointer of all the elements that populate the window container
* @return the created window container, or NULL on error
*/
DRGN_Window* drgn_windowNew(List* data, Vector2D pos);

/*
* @brief free the window container
* @param the window container to be freed
* @note window and data must be freed
*/
void drgn_windowFree(DRGN_Window* self);

/*
* @brief initalize the window management system
* @param max the maximum number of windows that can exist at one time
*/
void drgn_windowManagerNew(Uint32 max);

/*
* @brief closes the window manager
* @note each window object must be freed separately
*/
void drgn_windowManagerFree();

/*
* @brief draws the window
* @param the window to be drawn
*/
void drgn_windowDraw(DRGN_Window* self);

/*
* @brief draws all the windows currently in the window manager
*/
void drgn_windowManagerDraw();

#endif
