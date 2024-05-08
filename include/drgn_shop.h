#ifndef __DRGN_SHOP_H__
#define __DRGN_SHOP_H__

#include "drgn_window.h"
/*
*  @prupose create and manage in game shops and armories
*/

/*
* @brief creates a new shop
* @param name the name of the shop
*/
DRGN_Window* drgn_shopCreate(const char* name, DRGN_Entity* shopper);

/*
* @brief loads the shop file and json object and all its shop objects
* @param file the file to be loaded
*/
void drgn_shopFileInit(const char* file);

/*
* @brief closes the shop file and all the shop objects
* @note runs automatically when the game exits
*/
void drgn_shopFileClose();

/*
* @brief finds shop by its name
* @param name the name of the shop
*/
SJson*  drgn_shopGetDefByName(const char* name);
#endif
