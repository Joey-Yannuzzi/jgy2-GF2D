#include "simple_logger.h"
#include "drgn_unit.h"
#include "drgn_shop.h"

SJson* _shopFile = NULL;
SJson* _shops = NULL;

DRGN_Window* drgn_shopCreate(const char* name, DRGN_Entity* shopper)
{
	DRGN_Window* window;
	DRGN_Windel* text;
	DRGN_Windel* sellButton;
	SJson* shop;
	SJson* items;
	SJson* item;
	SJson* inventoryItem;
	int count, check;
	const char** itemNames;
	DRGN_Unit* unit;
	const char* inventoryName;
	DRGN_WindelButton* button;
	Color color;

	shop = drgn_shopGetDefByName(name);

	if (!shop)
	{
		slog("failed to find shop with name %s", name);
		return NULL;
	}

	items = sj_object_get_value(shop, "items");

	if (!items)
	{
		slog("failed to find shop items");
		return NULL;
	}

	count = sj_array_get_count(items);
	itemNames = gfc_allocate_array(sizeof(const char*), count);

	for (int bogus = 0; bogus < count; bogus++)
	{
		item = sj_array_get_nth(items, bogus);

		if (!item)
		{
			continue;
		}

		itemNames[bogus] = sj_get_string_value(item);
		slog("%s", itemNames[bogus]);
	}

	window = drgn_windowNew("shopWindow", 0, shopper, count, count);
	check = 0;

	for (int bogus = 0; bogus < window->elementsNum; bogus++)
	{
		if (!window->elements[bogus] || check > count)
		{
			continue;
		}

		button = (DRGN_WindelButton*)window->elements[bogus]->data;

		if (gfc_strlcmp(window->elements[bogus]->name, "buyButton") == 0)
		{
			window->elements[bogus]->selectable = 1;
			button->action = DRGN_BUTTON_BUY;
		}
		else if (gfc_strlcmp(window->elements[bogus]->name, "sellButton") == 0)
		{
			window->elements[bogus]->selectable = 1;
			button->action = DRGN_BUTTON_SELL;
		}

		if (gfc_strlcmp(window->elements[bogus]->name, "buyItems") == 0)
		{
			inventoryItem = drgn_inventoryGetDefByName(itemNames[check++]);
			inventoryName = sj_object_get_value_as_string(inventoryItem, "displayName");

			if (!inventoryName)
			{
				continue;
			}

			drgn_windelTextChangeText(window->elements[bogus], inventoryName);
		}
	}

	if (!shopper || !shopper->data)
	{
		slog("no unit is shopping currently");
		free(itemNames);
		return NULL;
	}

	unit = (DRGN_Unit*)shopper->data;

	if (unit->inventory->curr)
	{
		for (int bogus = 0; bogus < unit->inventory->curr; bogus++)
		{
			if (!&unit->inventory->itemList[bogus])
			{
				continue;
			}

			color = GFC_COLOR_WHITE;
			text = drgn_windelTextAdd(unit->inventory->itemList[bogus].name, vector2d(173, 128 + (bogus * 24)), window->pos, vector2d(1, 1), &color, unit->inventory->itemList[bogus].name, DRGN_MEDIUM_FONT);
			sellButton = drgn_windelButtonAdd("sellItemButton", vector2d(173, 128 + (bogus * 24)), window->pos, vector2d(1, 1), NULL, 0, shopper);

			if (!text)
			{
				continue;
			}

			if (!sellButton)
			{
				continue;
			}

			drgn_windowAddWindel(window, text);
			drgn_windowAddWindel(window, sellButton);
		}
	}
	free(itemNames);
	return (window);
}

void drgn_shopFileInit(const char* file)
{
	if (!file)
	{
		slog("no filename given");
		return;
	}

	_shopFile = sj_load(file);

	if (!_shopFile)
	{
		slog("shop file with name %s could not be opened", file);
		return;
	}

	_shops = sj_object_get_value(_shopFile, "shops");

	if (!_shops)
	{
		slog("could not find shops in file with name %s", file);
		sj_free(_shopFile);
		_shopFile = NULL;
		return;
	}
	
	atexit(drgn_shopFileClose);
}

void drgn_shopFileClose()
{
	if (_shopFile)
	{
		sj_free(_shopFile);
	}

	_shopFile = NULL;
	_shops = NULL;

	slog("Shop file closed successfully");
}

SJson* drgn_shopGetDefByName(const char* name)
{
	int count;
	SJson* shop;
	const char* tempName;

	if (!_shops)
	{
		slog("no shops to search through");
		return NULL;
	}

	if (!name)
	{
		slog("no name given");
		return NULL;
	}

	count = sj_array_get_count(_shops);

	for (int bogus = 0; bogus < count; bogus++)
	{
		shop = sj_array_get_nth(_shops, bogus);

		if (!shop)
		{
			continue;
		}

		tempName = sj_object_get_value_as_string(shop, "name");

		if (gfc_strlcmp(tempName, name) == 0)
		{
			return (shop);
		}
	}

	slog("Could not find shop with name %s", name);
	return NULL;
}