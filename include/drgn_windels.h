#ifndef __DRGN_WINDELS_H__
#define __DRGN_WINDELS_H__

#include "drgn_font.h"

/*
* @purpose windel creation, management, and destruction
*/
typedef struct
{
	const char* name; //name of the window element
	Vector2D pos; //position of the element relative to the window
	Vector2D scale; //scale of the element relative to the window; set to 1 for no additional scaling
	Color color; //color of the text
	void* data; //element specific data
}
DRGN_Windel;

typedef struct
{
	char* text; //the text to be displayed
	DRGN_FontStyles style; //the size of the font
}
DRGN_WindelText;

typedef struct
{
	Sprite* sprite; //the sprite to be drawn
	Uint8 useColor; //checks to see if color of sprite should be changed; set to 1 if color should be changed
	float frame; //frame of the sprite
}
DRGN_WindelSprite;

typedef struct
{
	Uint8 pushed; //checks if button has been pushed this frame; set to 1 if button was pushed
}
DRGN_WindelButton;

#endif