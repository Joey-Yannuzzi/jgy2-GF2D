#ifndef __DRGN_WINDELS_H__
#define __DRGN_WINDELS_H__

#include "gfc_types.h"
#include "gf2d_sprite.h"
#include "drgn_font.h"

/*
* @purpose windel creation, management, and destruction
*/

typedef struct DRGN_Windel_S
{
	const char* name; //name of the window element
	Vector2D pos; //position of the element relative to the window
	Vector2D scale; //scale of the element relative to the window; set to 1 for no additional scaling
	Color color; //color of the text
	void* data; //element specific data

	void (*free) (struct DRGN_Windel_S* windel); //call function for freeing windel
	void (*update) (struct DRGN_Windel_S* windel); //call function for updating windel
	void (*draw) (struct DRGN_Windel_S* windel); //call function for drawing windel
}
DRGN_Windel;

/*
* @brief create a new window element
* @param name the name of the element
* @param pos the position of the element local to the parent window
* @param scale OPTIONAL the scale of the element; NULL for no extra scaling
* @param color OPTIONAL the color the element should be set to; NULL for no additional color
* @return a pointer to the created window element
*/
DRGN_Windel* drgn_windelNew(const char* name, Vector2D pos, Vector2D* scale, Color* color);

/*
* @brief free a window element and its data
* @param windel the window element to be freed
*/
void drgn_windelFree(DRGN_Windel* windel);

/*
* @brief compute any actions a window element needs to take each frame
* @param windel the window element to be updated
*/
void drgn_windelUpdate(DRGN_Windel* windel);

/*
* @brief draw the window element each frame
* @param windel the window element to be drawn
*/
void drgn_windelDraw(DRGN_Windel* windel);

/*
* @purpose struct for text window elements
*/
typedef struct
{
	char* text; //the text to be displayed
	DRGN_FontStyles style; //the size of the font
}
DRGN_WindelText;

/*
* @brief create a new text windel
* @param name the name of the element
* @param pos the position of the element
* @param scale OPTIONAL the scale of the element; NULL for no additional scaling
* @param color OPTIONAL the color of the element; NULL for no color change
* @param text the text the element should render
* @param style the font size of the element
* @return the created window element
*/
DRGN_Windel* drgn_windelTextNew(const char* name, Vector2D pos, Vector2D* scale, Color* color, char* text, DRGN_FontStyles style);

/*
* @brief frees a text window element
* @param windel the window element to be freed
*/
void drgn_windelTextFree(DRGN_Windel* windel);

/*
* @brief updates a text window element
* @param windel the window element to be updated
*/
void drgn_windelTextUpdate(DRGN_Windel* windel);

/*
* @brief draws a text window element
* @param windel the window element to be drawn
*/
void drgn_windelTextDraw(DRGN_Windel* windel);

/*
* @purpose struct for sprite window elements
*/
typedef struct
{
	Sprite* sprite; //the sprite to be drawn
	Uint8 useColor; //checks to see if color of sprite should be changed; set to 1 if color should be changed
	float frame; //frame of the sprite
}
DRGN_WindelSprite;

/*
* @brief creates a new sprite window element
* @param name the name of the element
* @param pos the position of the element local to the parent window
* @param scale OPTIONAL the scale of the element; NULL for no extra scaling
* @param color OPTIONAL the color the element should be set to; NULL for no additional color
* @param sprite the sprite to be rendered
* @return the created window element
*/
DRGN_Windel* drgn_windelSpriteNew(const char* name, Vector2D pos, Vector2D* scale, Color* color, Sprite* sprite);

/*
* @brief frees a sprite window element
* @param windel the window element to be freed
*/
void drgn_windelSpriteFree(DRGN_Windel* windel);

/*
* @brief updates a sprite window element
* @param the window element to be updated
*/
void drgn_windelSpriteUpdate(DRGN_Windel* windel);

/*
* @brief draws a sprite window element
* @param the window element to be drawn
*/
void drgn_windelSpriteDraw(DRGN_Windel* windel);

/*
* @purpose struct for button window elements
*/
typedef struct
{
	Uint8 pushed; //checks if button has been pushed this frame; set to 1 if button was pushed
}
DRGN_WindelButton;

/*
* @brief creates a new button window element
* @param name the name of the element
* @param pos the position of the element local to the parent window
* @param scale OPTIONAL the scale of the element; NULL for no extra scaling
* @param color OPTIONAL the color the element should be set to; NULL for no additional color
* @return the created window element
*/
DRGN_Windel* drgn_windelButtonNew(const char* name, Vector2D pos, Vector2D* scale, Color* color);

/*
* @brief frees a button window element
* @param the window element to be freed
*/
void drgn_windelButtonFree(DRGN_Windel* windel);

/*
* @brief updates a button window element
* @param the window element to be updated
*/
void drgn_windelButtonUpdate(DRGN_Windel* windel);

/*
* @brief draws a button window element
* @param the window element to be drawn
*/
void drgn_windelButtonDraw(DRGN_Windel* windel);
#endif