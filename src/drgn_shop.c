#include "simple_logger.h"
#include "drgn_window.h"
#include "drgn_shop.h"

SJson* _shopFile = NULL;
SJson* _shops = NULL;

void drgn_shopCreate(const char* name)
{
	DRGN_Window* window;
	SJson* shop;
	SJson* items;
	SJson* item;
	int count;
	const char** itemNames;

	shop = drgn_shopGetDefByName(name);

	if (!shop)
	{
		slog("failed to find shop with name %s", name);
		return;
	}

	items = sj_object_get_value(shop, "items");

	if (!items)
	{
		slog("failed to find shop items");
		return;
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

	window = drgn_windowNew("shopWindow", 0, NULL, count);
	free(itemNames);
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