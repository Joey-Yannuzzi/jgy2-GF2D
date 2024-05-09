#include "simple_logger.h"
#include "drgn_inventory.h"

static SJson* _inventoryJson = NULL;
static SJson* _inventoryItems = NULL;

void drgn_inventoryFileInit(const char* file)
{
	if (!file)
	{
		slog("No filename given");
		return;
	}

	_inventoryJson = sj_load(file);

	if (!_inventoryJson)
	{
		slog("No file for filename %s", file);
		return;
	}

	_inventoryItems = sj_object_get_value(_inventoryJson, "items");

	if (!_inventoryItems)
	{
		slog("No inventory items in specified file %s", file);
		sj_free(_inventoryJson);
		_inventoryJson = NULL;
		return;
	}
	atexit(drgn_inventoryClose);
}

SJson* drgn_inventoryGetDefByName(const char* name)
{
	SJson* item;
	int count;
	const char* itemName;
	const char* displayName;

	if (!name)
	{
		slog("No name specified");
		return NULL;
	}

	if (!_inventoryItems)
	{
		slog("No items list available to check");
		return NULL;
	}

	count = sj_array_get_count(_inventoryItems);
	slog("counted item number");
		
	for (int bogus = 0; bogus < count; bogus++)
	{
		item = sj_array_get_nth(_inventoryItems, bogus);
		slog("got nth item");

		if (!item)
		{
			continue;
		}

		itemName = sj_object_get_value_as_string(item, "name");
		displayName = sj_object_get_value_as_string(item, "displayName");
		slog("got name %s", itemName);

		if (!itemName)
		{
			continue;
		}

		if (gfc_strlcmp(name, itemName) == 0 || gfc_strlcmp(name, displayName) == 0)
		{
			return (item);
		}
	}
}

void drgn_inventoryClose()
{
	if (_inventoryJson)
	{
		sj_free(_inventoryJson);
	}

	_inventoryJson = NULL;
	_inventoryItems = NULL;

	slog("freed inventory file successfully");
}

DRGN_Inventory* drgn_inventoryNew(const char* itemNames[], int curr, int max)
{
	DRGN_Inventory* self;
	DRGN_InventoryItem* item;

	if (_inventoryItems == NULL)
	{
		slog("No item list initialized");
		return NULL;
	}
	self = gfc_allocate_array(sizeof(DRGN_Inventory), 1);

	if (!self)
	{
		slog("No inventory to ber initialized");
		return NULL;
	}

	self->max = max;
	self->itemList = gfc_allocate_array(sizeof(DRGN_InventoryItem), max);

	if (!self->itemList)
	{
		slog("No item list was allocated");
		return NULL;
	}

	self->equipped = 0;
	self->curr = 0;
	slog("Successfully created inventory");

	if (itemNames)
	{
		for (int bogus = 0; bogus < curr; bogus++)
		{
			if (!itemNames[bogus])
			{
				continue;
			}

			self->curr++;
			item = drgn_inventoryItemNew(itemNames[bogus]);
			
			if (!item)
			{
				slog("failed to get item from name");
				continue;
			}

			self->itemList[bogus] = *item;
		}
	}

	slog("Gave inventory %i items", self->curr);
	return (self);
}

void drgn_inventoryFree(DRGN_Inventory* self)
{
	if (!self)
	{
		slog("not self");
		return;
	}

	if (!self->itemList)
	{
		slog("no itemlist");
		return;
	}

	for (int bogus = 0; bogus < self->curr; bogus++)
	{
		if (!&self->itemList[bogus])
		{
			continue;
		}

		drgn_inventoryItemFree(&self->itemList[bogus]);
	}

	slog("got here");
	//free(&self->itemList);
	self = NULL;
}

DRGN_InventoryItem* drgn_inventoryItemNew(const char* name)
{
	SJson* obj;
	DRGN_InventoryItem* item;
	DRGN_InventoryItemPotion* potion;
	DRGN_InventoryItemStatBooster* statBooster;
	const char* displayName;

	if (!name)
	{
		slog("No name given to search");
		return NULL;
	}

	slog("about to get json object");
	obj = drgn_inventoryGetDefByName(name);

	if (!obj)
	{
		slog("could not find item with name %s", name);
		return NULL;
	}

	item = gfc_allocate_array(sizeof(DRGN_InventoryItem), 1);

	if (!item)
	{
		slog("Failed to allocate memory for the item");
		return NULL;
	}
	gfc_line_cpy(item->name, sj_object_get_value_as_string(obj, "displayName"));
	gfc_line_cpy(item->description, sj_object_get_value_as_string(obj, "description"));
	slog("here");
	slog("about to make item");
	sj_object_get_value_as_int(obj, "uses", &item->uses);
	sj_object_get_value_as_int(obj, "cost", &item->cost);
	sj_object_get_value_as_int(obj, "type", &item->type);
	item->_inuse = 1;

	if (!item->type)
	{
		slog("No item type");
		return NULL;
	}

	switch (item->type)
	{
	case DRGN_POTION:
		potion = gfc_allocate_array(sizeof(DRGN_InventoryItemPotion), 1);
		sj_object_get_value_as_int(obj, "heal", &potion->heal);

		if (!potion->heal)
		{
			slog("no heal value");
			return NULL;
		}

		item->data = potion;
		slog("Potion successfully created");
		break;
	case DRGN_STAT_BOOSTER:
		statBooster = gfc_allocate_array(sizeof(DRGN_InventoryItemStatBooster), 1);
		sj_object_get_value_as_int(obj, "stat", &statBooster->stat);
		sj_object_get_value_as_int(obj, "increase", &statBooster->increase);
		item->data = statBooster;
		slog("Stat booster successfully created");
		break;
	case DRGN_ARCANE:
		slog("Spell book created");
	default:
		break;
	}

	slog("Item with the name %s has been created", item->name);
	return (item);
}

void drgn_inventoryItemFree(DRGN_InventoryItem* self)
{
	DRGN_InventoryItemPotion* potion;
	DRGN_InventoryItemStatBooster* statBooster;

	if (!self)
	{
		return;
	}

	self->_inuse = 0;

	if (!self->data)
	{
		return;
	}

	if (self->type == DRGN_POTION)
	{
		potion = (DRGN_InventoryItemPotion*)self->data;
		free(potion);
	}
	else if (self->type == DRGN_STAT_BOOSTER)
	{
		statBooster = (DRGN_InventoryItemStatBooster*)self->data;
		free(statBooster);
	}
}

void drgn_inventoryItemAdd(DRGN_Inventory* self, const char* name)
{
	DRGN_InventoryItem* item;
	if (!self)
	{
		return;
	}

	if (!name)
	{
		slog("no name given");
		return;
	}

	if (self->curr >= self->max)
	{
		slog("Must drop an item before picking up new one");
		return;
	}

	item = drgn_inventoryItemNew(name);

	if (!item)
	{
		slog("No item could be allocated");
		return;
	}

	self->itemList[self->curr - 1] = *item;
	self->curr++;
	slog("Successfully added item to spot %i", self->curr);
}

DRGN_InventoryItem* drgn_inventoryCheckItemTypeInInventory(DRGN_Inventory* self, enum DRGN_InvetoryItemType type)
{
	if (!self)
	{
		slog("No inventory given");
		return NULL;
	}

	if (!type)
	{
		slog("No type specified");
		return NULL;
	}


	for (int bogus = 0; bogus < self->curr; bogus++)
	{
		slog("Item: %s", self->itemList[bogus].name);

		if (self->itemList[bogus].type == type)
		{
			return (&self->itemList[bogus]);
		}
	}

	slog("failed to find item");
	return NULL;
}

void drgn_inventoryItemRemove(DRGN_Inventory* self, int index)
{
	if (!self)
	{
		slog("No inventory given");
		return;
	}

	if (index < 0 || index >= self->curr)
	{
		slog("invalid index %i for current item count %i", index, self->curr);
		return;
	}

	if (self->curr <= 0)
	{
		self->curr = 0;
		slog("No items in this inventory to remove");
		return;
	}

	drgn_inventoryItemFree(&self->itemList[index]);
	self->curr--;
}