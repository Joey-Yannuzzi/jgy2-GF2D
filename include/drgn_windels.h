#ifndef __DRGN_WINDELS_H__
#define __DRGN_WINDELS_H__

#include "simple_json.h"
//#include "gfc_types.h"
//#include "gf2d_sprite.h"
#include "drgn_font.h"
#include "drgn_entity.h"

/*
* @purpose windel creation, management, and destruction
*/

typedef struct DRGN_Windel_S
{
	const char* name; //name of the window element
	Vector2D pos; //position of the element relative to the window
	Vector2D scale; //scale of the element relative to the window; set to 1 for no additional scaling
	Color color; //color of the text
	Uint8 selectable; //is this windel able to be selected
	void* data; //element specific data

	void (*free) (struct DRGN_Windel_S* windel); //call function for freeing windel
	void (*update) (struct DRGN_Windel_S* windel); //call function for updating windel
	void (*draw) (struct DRGN_Windel_S* windel); //call function for drawing windel
}
DRGN_Windel;

/*
* @brief create a new window element
* @param object the json object to pull data from
* @return a pointer to the created window element
*/
DRGN_Windel* drgn_windelNew(SJson* object, Vector2D parentPos);

DRGN_Windel* drgn_windelAdd(const char* name, Vector2D pos, Vector2D parentPos, Vector2D scale, Color* color);

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
* @param object the json object to pull data from
* @return the created window element
*/
DRGN_Windel* drgn_windelTextNew(SJson* object, Vector2D parentPos);

/*
* @brief used to directly create new text windels to an existing window
*/
DRGN_Windel* drgn_windelTextAdd(const char* name, Vector2D pos, Vector2D parentPos, Vector2D scale, Color* color, const char* text, DRGN_FontStyles style);

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

void drgn_windelTextChangeText(DRGN_Windel* windel, const char* newText);

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
DRGN_Windel* drgn_windelSpriteNew(SJson* object, Vector2D parentPos);

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

Uint32 drgn_windelSpriteGetWidth(DRGN_Windel* windel);

Uint32 drgn_windelSpriteGetHeight(DRGN_Windel* windel);


typedef enum DRGN_CommandAction
{
	DRGN_BUTTON_NONE,
	DRGN_BUTTON_SEIZE,
	DRGN_BUTTON_TALK,
	DRGN_BUTTON_MELEE_ATTACK,
	DRGN_BUTTON_RANGED_ATTACK,
	DRGN_BUTTON_MAGIC_ATTACK,
	DRGN_BUTTON_HEAL,
	DRGN_BUTTON_ITEM,
	DRGN_BUTTON_TRADE,
	DRGN_BUTTON_RESCUE,
	DRGN_BUTTON_TRANSFER,
	DRGN_BUTTON_DROP,
	DRGN_BUTTON_WAIT,
	DRGN_BUTTON_ITEM_SHOP,
	DRGN_BUTTON_ARMORY,
	DRGN_BUTTON_BUY,
	DRGN_BUTTON_SELL
}
DRGN_ButtonAction;

/*
* @purpose struct for button window elements
*/
typedef struct
{
	Uint8 pushed; //checks if button has been pushed this frame; set to 1 if button was pushed
	DRGN_ButtonAction action; //action the button should perform
	DRGN_Entity* parent; //parent entity of the button, if it exists
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
DRGN_Windel* drgn_windelButtonNew(SJson* object, Vector2D parentPos, DRGN_ButtonAction action, DRGN_Entity* parent);

DRGN_Windel* drgn_windelButtonAdd(const char* name, Vector2D pos, Vector2D parentPos, Vector2D scale, Color* color, DRGN_ButtonAction action, DRGN_Entity* parent);

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

void drgn_windelButtonCompleteAction(DRGN_WindelButton* button);

#endif