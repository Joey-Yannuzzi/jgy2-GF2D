#ifndef __DRGN_INVENTORY_H__
#define __DRGN_INVENTORY_H__

#include "simple_json.h"
#include "gfc_text.h"

enum DRGN_InventoryItemType
{
	DRGN_NONE,
	DRGN_POTION,
	DRGN_STAT_BOOSTER,
	DRGN_SWORD,
	DRGN_LANCE,
	DRGN_AXE,
	DRGN_BOW,
	DRGN_RAPIER,
	DRGN_DAGGER,
};

typedef struct
{
	Uint8 _inuse;
	TextLine name; //name of the item
	TextLine description; //description of the item
	int uses; //how many uses the item has
	int cost; //how much the item costs in the shop; half as much when selling
	enum DRGN_InventoryItemType type; //item type
	void* data; //item specific information, like weapon data
}
DRGN_InventoryItem;

typedef struct
{
	DRGN_InventoryItem* itemList; //list of the items in inventory
	int equipped; //item number in list that is equipped
	int max; //max items that can be held
	int curr; //current number of items being held
}
DRGN_Inventory;

typedef struct
{
	int stat; //the index to the stat to be increased
	int increase; //the number the stat is increased by
}
DRGN_InventoryItemStatBooster;

typedef struct
{
	int heal; //amount potion should heal for
}
DRGN_InventoryItemPotion;

/*
* @brief Initializes inventory items from a file
* @param file the name of the file to be chosen
*/
void drgn_inventoryFileInit(const char* file);

/*
* @brief finds an inventory item by its name
* @param name the name of the item to be found
* @return a SJson object pointer to the definition of the item; NULL if it does not exist or there is an error
* @note data does not need to be freed
*/
SJson* drgn_inventoryGetDefByName(const char* name);

/*
* @brief closes the file opened in init function
*/
void drgn_inventoryClose();

DRGN_Inventory* drgn_inventoryNew(const char* itemNames, int max);

void drgn_inventoryAddItem(DRGN_Inventory* self, const char* name);

void drgn_inventoryFree(DRGN_Inventory* self);

DRGN_InventoryItem* drgn_inventoryItemNew(const char* name);

void drgn_inventoryItemFree(DRGN_InventoryItem* self);

void drgn_inventoryItemAdd(DRGN_Inventory* self, const char* name);

/*
* @brief determines if an item of a specific type exists in an inventory
* @param self the inventory being checked
* @param type the desired item type
* @return the first instance of the inventory item with the desired type; NULL if none exists or error occurs
*/
DRGN_InventoryItem* drgn_inventoryCheckItemTypeInInventory(DRGN_Inventory* self, enum DRGN_InventoryItemType type);

void drgn_inventoryItemRemove(DRGN_Inventory* self, int index);

#endif
