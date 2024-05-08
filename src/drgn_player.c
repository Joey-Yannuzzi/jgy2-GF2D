#include "simple_logger.h"
#include "drgn_player.h"
#include "drgn_camera.h"
#include "drgn_terrain.h"
#include "drgn_unit.h"
#include "drgn_windels.h"

DRGN_Entity* drgn_playerNew()
{
	DRGN_Entity* self;
	DRGN_Player* player;
	self = drgn_entityNew();

	if (!self)
	{
		slog("No free space for player entity exists");
		return NULL;
	}

	self->sprite = gf2d_sprite_load_all("images/newCursor.png", 64, 64, 4, 0);
	self->frame = 0;
	self->pos = vector2d(0, 0);
	self->scale = vector2d(1, 1);
	self->color = GFC_COLOR_LIGHTCYAN;
	self->colorSet = 1;
	self->affiliation = DRGN_CURSOR;
	self->think = drgn_playerThink;
	self->update = drgn_playerUpdate;
	self->free = drgn_playerFree;
	slog("Player entity spawned");
	//slog("%i", (*(DRGN_Player*)self->data).test);
	player = gfc_allocate_array(sizeof(DRGN_Player), 1);
	self->data = player;
	player->terrainWindow = drgn_windowNew("terrainDisplay", DRGN_BUTTON_NONE, NULL, 0, 0);
	player->unitWindow = NULL;
	player->targets = gfc_allocate_array(sizeof(DRGN_Entity*), 12);
	return (self);
}

void drgn_playerThink(DRGN_Entity* self)
{
	const Uint8* keys;
	Vector2D dir = { 0 };
	SDL_Event event;
	Vector2D offset = { 0 };
	DRGN_Player* player;

	SDL_PumpEvents();
	keys = SDL_GetKeyboardState(NULL);
	SDL_PollEvent(&event);

	if (!self || !self->data)
	{
		slog("No player to update");
		return;
	}

	player = (DRGN_Player*)self->data;

	if (player->statScreenPress && keys[SDL_SCANCODE_E] && event.type == SDL_KEYDOWN)
	{
		player->statScreenPress = 0;
	}

	if (player->targeting)
	{
		if ((keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_D]) && event.type == SDL_KEYDOWN)
		{
			player->currentTarget++;
		}
		else if ((keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_A]) && event.type == SDL_KEYDOWN)
		{
			player->currentTarget--;
		}

		if (keys[SDL_SCANCODE_SPACE] && !player->pressed && event.type == SDL_KEYDOWN)
		{
			player->pressed = 1;
		}

		return;
	}

	if (self->inactive)
	{
		return;
	}

	if (keys[SDL_SCANCODE_W] && event.type == SDL_KEYDOWN)
	{
		dir.y = -1;
	}
	else if (keys[SDL_SCANCODE_S] && event.type == SDL_KEYDOWN)
	{
		dir.y = 1;
	}
	else if (keys[SDL_SCANCODE_A] && event.type == SDL_KEYDOWN)
	{
		dir.x = -1;
	}
	else if (keys[SDL_SCANCODE_D] && event.type == SDL_KEYDOWN)
	{
		dir.x = 1;
	}

	if (keys[SDL_SCANCODE_SPACE] && !player->pressed && event.type == SDL_KEYDOWN)
	{
		player->pressed = 1;
		//self->selected = 1;
		//slog("begin selection");
	}

	if (keys[SDL_SCANCODE_E] && !player->statScreenPress && !player->pressed && event.type == SDL_KEYDOWN && !player->statScreen)
	{
		player->statScreenPress = 1;
	}

	vector2d_normalize(&dir);

	if (dir.x)
	{
		offset.x = self->scale.x * self->sprite->frame_w * dir.x;
	}
	if (dir.y)
	{
		offset.y = self->scale.y * self->sprite->frame_h * dir.y;
	}

	vector2d_copy(self->velocity, offset);
}

void drgn_playerUpdate(DRGN_Entity* self)
{
	DRGN_Entity* unit;
	DRGN_Entity* red;
	DRGN_Entity* green;
	DRGN_Entity* terrain;
	DRGN_Terrain* terrainData;
	DRGN_Player* player;
	Rect bounds;
	DRGN_Unit* curr;
	DRGN_Window* window;
	DRGN_Windel* windel;
	size_t size;
	char* terrainText;
	char* text;
	int x, y;

	if (!self || !self->data)
	{
		slog("No player to have thoughts");
		return;
	}

	self->frame += 0.05;

	if (self->frame > 4)
	{
		self->frame = 0;
	}

	player = (DRGN_Player*)self->data;

	if (player->statScreenPress && !player->statScreen)
	{
		//player->statScreenPress = 0;
		unit = drgn_entityGetSelectionByPosition(DRGN_BLUE, self->pos, self);

		if (!unit || !unit->data)
		{
			player->statScreenPress = 0;
			return;
		}

		player->statScreen = drgn_windowNew("statScreen", DRGN_BUTTON_NONE, NULL, 0, 0);
		self->inactive = 1;
		curr = (DRGN_Unit*)unit->data;

		if (player->statScreen)
		{
			drgn_windowChangePosition(player->statScreen, vector2d(unit->pos.x + 64, unit->pos.y));

			for (int bogus = 0; bogus < player->statScreen->elementsNum; bogus++)
			{
				if (!player->statScreen->elements[bogus] || !player->statScreen->elements[bogus]->name)
				{
					continue;
				}

				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "unitName") == 0)
				{
					drgn_windelTextChangeText(player->statScreen->elements[bogus], curr->name);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "hp") == 0)
				{
					text = gfc_allocate_array(sizeof("HP: ") + sizeof(curr->stats[1]), 1);
					sprintf(text, "HP: %i", curr->stats[1]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "str") == 0)
				{
					text = gfc_allocate_array(sizeof("Strength: ") + sizeof(curr->stats[2]), 1);
					sprintf(text, "Strenth: %i", curr->stats[2]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "mag") == 0)
				{
					text = gfc_allocate_array(sizeof("Magic: ") + sizeof(curr->stats[3]), 1);
					sprintf(text, "Magic: %i", curr->stats[3]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "skl") == 0)
				{
					text = gfc_allocate_array(sizeof("Skill: ") + sizeof(curr->stats[4]), 1);
					sprintf(text, "Skill: %i", curr->stats[4]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "spd") == 0)
				{
					text = gfc_allocate_array(sizeof("Speed: ") + sizeof(curr->stats[5]), 1);
					sprintf(text, "Speed: %i", curr->stats[5]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "lck") == 0)
				{
					text = gfc_allocate_array(sizeof("Luck: ") + sizeof(curr->stats[6]), 1);
					sprintf(text, "Luck: %i", curr->stats[6]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "def") == 0)
				{
					text = gfc_allocate_array(sizeof("Defense: ") + sizeof(curr->stats[7]), 1);
					sprintf(text, "Defense: %i", curr->stats[7]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "res") == 0)
				{
					text = gfc_allocate_array(sizeof("Resistance: ") + sizeof(curr->stats[8]), 1);
					sprintf(text, "Resistance: %i", curr->stats[8]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "mov") == 0)
				{
					text = gfc_allocate_array(sizeof("Move: ") + sizeof(curr->stats[9]), 1);
					sprintf(text, "Move: %i", curr->stats[9]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
				if (gfc_strlcmp(player->statScreen->elements[bogus]->name, "bld") == 0)
				{
					text = gfc_allocate_array(sizeof("Build: ") + sizeof(curr->stats[10]), 1);
					sprintf(text, "Build: %i", curr->stats[10]);
					drgn_windelTextChangeText(player->statScreen->elements[bogus], text);
				}
			}
		}

		return;
	}

	if (!player->statScreenPress && player->statScreen)
	{
		drgn_windowFree(player->statScreen);
		player->statScreen = NULL;
		self->inactive = 0;
	}

	if (player->targeting)
	{
		if (player->currentTarget >= player->totalTargets)
		{
			player->currentTarget = 0;
		}
		else if (player->currentTarget < 0)
		{
			player->currentTarget = player->totalTargets - 1;
		}

		vector2d_copy(self->pos, player->targets[player->currentTarget]->pos);


		if (player->pressed)
		{
			drgn_unitInteractionByEnum(self->curr, player->targets[player->currentTarget]);
			player->pressed = 0;
		}
		return;
	}

	if (self->inactive)
	{
		if (player->unitWindow)
		{
			drgn_entityFree(player->unitWindow);
		}

		player->unitWindow = NULL;
		player->unitWindowSource = NULL;
		return;
	}

	vector2d_add(self->pos, self->pos, self->velocity);
	bounds = drgn_cameraGetBounds();

	if ((self->pos.x + (self->scale.x * self->sprite->frame_w)) > (bounds.w + bounds.x))
	{
		self->pos.x = (bounds.w + bounds.x) - (self->scale.x * self->sprite->frame_w);
	}
	if ((self->pos.y + (self->scale.y * self->sprite->frame_h)) > (bounds.h + bounds.y))
	{
		self->pos.y = (bounds.h + bounds.y) - (self->scale.y * self->sprite->frame_h);
	}
	if (self->pos.x < bounds.x)
	{
		self->pos.x = bounds.x;
	}
	if (self->pos.y < bounds.y)
	{
		self->pos.y = bounds.y;
	}

	drgn_cameraCenterOn(self->pos);

	if (player->pressed  && !self->curr)
	{
		//slog("unit selected");
		unit = drgn_entityGetSelectionByPosition(DRGN_BLUE, self->pos, self);
		player->pressed = 0;

		if (!unit)
		{
			return;
		}

		curr = (DRGN_Unit*)unit->data;

		if (!curr->active)
		{
			return;
		}

		self->selected = 1;
		self->curr = unit;
		unit->selected = 1;
		unit->color = GFC_COLOR_CYAN;
	}
	else if (player->pressed && self->curr && self->selected && drgn_entityGetSelectionByPosition(DRGN_TILE, self->pos, self))
	{

		self->curr->pos = self->pos;
		curr = (DRGN_Unit*)self->curr->data;
		curr->currentAction = DRGN_MOVE;
		drgn_unitMenu(self->curr);
		self->curr->curr = self;
		self->inactive = 1;
		self->curr = NULL;
		self->selected = 0;
		player->pressed = 0;
	}
	else if (player->pressed && self->curr && self->selected && drgn_entityGetSelectionByPosition(DRGN_BLUE, self->pos, self->curr))
	{
		curr = (DRGN_Unit*)drgn_entityGetSelectionByPosition(DRGN_BLUE, self->pos, self->curr)->data;

		if (!curr->active)
		{
			self->curr->color = GFC_COLOR_BLUE;
			self->curr->selected = 0;
			drgn_unitMoveFree(self->curr);
			self->curr = NULL;
			self->selected = 0;
			player->pressed = 0;
			return;
		}
		self->curr->color = GFC_COLOR_BLUE;
		self->curr->selected = 0;
		drgn_unitMoveFree(self->curr);
		self->curr = drgn_entityGetSelectionByPosition(DRGN_BLUE, self->pos, self->curr);
		self->curr->selected = 1;
		self->curr->color = GFC_COLOR_CYAN;
		player->pressed = 0;
	}
	else if (player->pressed && self->curr && self->selected)
	{
		self->curr->color = GFC_COLOR_BLUE;
		self->curr->selected = 0;
		drgn_unitMoveFree(self->curr);
		self->curr = NULL;
		self->selected = 0;
		player->pressed = 0;
	}
	else
	{
		player->pressed = 0;
	}

	/*if (drgn_entityGetSelectionByPosition(DRGN_BLUE, self->pos, self) && !player->unitWindow && !player->unitWindowSource && !self->selected)
	{
		//slog("begining to draw");
		player->unitWindowSource = drgn_entityGetSelectionByPosition(DRGN_BLUE, self->pos, self);
		curr = (DRGN_Unit*)player->unitWindowSource->data;

		if (self->pos.x - 64 < 0)
		{
			x = self->pos.x;
		}
		else if ((self->pos.x + (self->scale.x * self->sprite->frame_w)) + 64 > (bounds.w + bounds.x))
		{
			x = self->pos.x - 128;
		}
		else
		{
			x = self->pos.x - 64;
		}

		if (self->pos.y - 64 < 0)
		{
			y = self->pos.y + 64;
		}
		else
		{
			y = self->pos.y - 64;
		}

		//player->unitWindow = drgn_windowNew(curr->name, "images/windows/unitWindow.png", 192, 64, vector2d(x, y), NULL);
		player->unitWindow->offset = 1;
		//window->texts = curr->name;
		slog("created unit window");
	}
	else if (drgn_entityGetSelectionByPosition(DRGN_RED, self->pos, self) && !player->unitWindow && !player->unitWindowSource && !self->selected)
	{
		slog("begining to draw");
		player->unitWindowSource = drgn_entityGetSelectionByPosition(DRGN_RED, self->pos, self);
		curr = (DRGN_Unit*)player->unitWindowSource->data;

		if (self->pos.x - 64 < 0)
		{
			x = self->pos.x;
		}
		else if ((self->pos.x + (self->scale.x * self->sprite->frame_w)) + 64 > (bounds.w + bounds.x))
		{
			x = self->pos.x - 128;
		}
		else
		{
			x = self->pos.x - 64;
		}

		if (self->pos.y - 64 < 0)
		{
			y = self->pos.y + 64;
		}
		else
		{
			y = self->pos.y - 64;
		}

		//player->unitWindow = drgn_windowNew(curr->name, "images/windows/unitWindow.png", 192, 64, vector2d(x, y), NULL);
		player->unitWindow->offset = 1;
		//window->texts = curr->name;
		slog("created unit window");
	}
	else if (drgn_entityGetSelectionByPosition(DRGN_GREEN, self->pos, self) && !player->unitWindow && !player->unitWindowSource && !self->selected)
	{
		//slog("begining to draw");
		player->unitWindowSource = drgn_entityGetSelectionByPosition(DRGN_GREEN, self->pos, self);
		curr = (DRGN_Unit*)player->unitWindowSource->data;

		if (self->pos.x - 64 < 0)
		{
			x = self->pos.x;
		}
		else if ((self->pos.x + (self->scale.x * self->sprite->frame_w)) + 64 > (bounds.w + bounds.x))
		{
			x = self->pos.x - 128;
		}
		else
		{
			x = self->pos.x - 64;
		}

		if (self->pos.y - 64 < 0)
		{
			y = self->pos.y + 64;
		}
		else
		{
			y = self->pos.y - 64;
		}

		//player->unitWindow = drgn_windowNew(curr->name, "images/windows/unitWindow.png", 192, 64, vector2d(x, y), NULL);
		player->unitWindow->offset = 1;
		//window->texts = curr->name;
		slog("created unit window");
	}
	else if ((player->unitWindow && player->unitWindowSource && !vector2d_compare(self->pos, player->unitWindowSource->pos)) || self->selected)
	{
		if (player->unitWindow)
		{
			drgn_entityFree(player->unitWindow);
		}

		player->unitWindow = NULL;
		player->unitWindowSource = NULL;
		//slog("deleted unit window");
	}*/

	windel = NULL;

	for (int bogus = 0; bogus < player->terrainWindow->elementsNum; bogus++)
	{
		windel = player->terrainWindow->elements[bogus];

		if (!windel)
		{
			continue;
		}

		if (gfc_strlcmp(windel->name, "backgroundSprite") == 0)
		{
			break;
		}
	}

	if (self->pos.x > player->terrainWindow->pos.x)
	{
		//slog("one");
		//player->terrainWindow->pos.x = 80;
		drgn_windowChangePosition(player->terrainWindow, vector2d(80, player->terrainWindow->pos.y));
	}
	if (self->pos.x < player->terrainWindow->pos.x + drgn_windelSpriteGetWidth(windel))
	{
		//slog("two");
		//player->terrainWindow->pos.x = 1024;
		drgn_windowChangePosition(player->terrainWindow, vector2d(1024, player->terrainWindow->pos.y));
	}

	if (self->pos.y > player->terrainWindow->pos.y)
	{
		//slog("three");
		//player->terrainWindow->pos.y = 36;
		drgn_windowChangePosition(player->terrainWindow, vector2d(player->terrainWindow->pos.x, 36));
	}
	if (self->pos.y < player->terrainWindow->pos.y + drgn_windelSpriteGetHeight(windel))
	{
		//slog("four");
		//player->terrainWindow->pos.y = 576;
		drgn_windowChangePosition(player->terrainWindow, vector2d(player->terrainWindow->pos.x, 576));
	}

	terrain = drgn_entityGetSelectionByPosition(0, self->pos, self);
	window = (DRGN_Window*)player->terrainWindow;
	
	if (!terrain)
	{
		//slog("Player not on any known terrain");
		
		for (int bogus = 0; bogus < window->elementsNum; bogus++)
		{
			if (gfc_strlcmp(window->elements[bogus]->name, "terrainValue") == 0)
			{
				drgn_windelTextChangeText(window->elements[bogus], "grass");
			}
			else if (gfc_strlcmp(window->elements[bogus]->name, "terrainValueAvoid") == 0)
			{
				drgn_windelTextChangeText(window->elements[bogus], "Avoid: 0");
			}
			else if (gfc_strlcmp(window->elements[bogus]->name, "terrainValueDefense") == 0)
			{
				drgn_windelTextChangeText(window->elements[bogus], "Defense: 0");
			}
		}
		return;
	}

	terrainData = (DRGN_Terrain*)terrain->data;

	if (!terrainData)
	{
		//slog("No data on terrain");
		return;
	}

	for (int bogus = 0; bogus < window->elementsNum; bogus++)
	{
		if (gfc_strlcmp(window->elements[bogus]->name, "terrainValue") == 0)
		{
			drgn_windelTextChangeText(window->elements[bogus], terrainData->name);
		}
		else if (gfc_strlcmp(window->elements[bogus]->name, "terrainValueAvoid") == 0)
		{
			terrainText = gfc_allocate_array(sizeof("Avoid: ") + sizeof(terrainData->avoidBonus), 1);
			sprintf(terrainText, "Avoid: %i", terrainData->avoidBonus);
			drgn_windelTextChangeText(window->elements[bogus],terrainText);
		}
		else if (gfc_strlcmp(window->elements[bogus]->name, "terrainValueDefense") == 0)
		{
			terrainText = gfc_allocate_array(sizeof("Defense: ") + sizeof(terrainData->defBonus), 1);
			sprintf(terrainText, "Defense: %i", terrainData->defBonus);
			drgn_windelTextChangeText(window->elements[bogus], terrainText);
		}
	}
	//size = strlen(terrainData->name) + strlen("Avoid: ")  + sizeof(terrainData->avoidBonus) + sizeof(terrainData->defBonus) + 1;
	//sprintf(window->texts, "%s\nAvoid: %i\nDefense: %i", terrainData->name, terrainData->avoidBonus, terrainData->defBonus);
}

void drgn_playerFree(DRGN_Entity* self)
{
	DRGN_Player* player;
	DRGN_Window* window;

	if (!self || !self->data)
	{
		slog("No player to be freed");
		return;
	}

	player = (DRGN_Player*)self->data;

	if (player->targets)
	{
		free(player->targets);
	}
}

DRGN_Entity* drgn_playerCheckSelectionByPosition(DRGN_Entity* selector, DRGN_Entity* army)
{
	int bogus = 0;

	while (&army[bogus])
	{
		if (!army[bogus]._inuse)
		{
			bogus++;
			continue;
		}

		if (selector->affiliation != army->affiliation)
		{
			bogus++;
			continue;
		}

		if  (army->pos.x == selector->pos.x && army->pos.y == selector->pos.y)
		{
			return (&army[bogus]);
		}

		bogus++;
	}

	return NULL;
}

void drgn_playerCheckBounds(DRGN_Entity* self, Rect bounds)
{
	if ((self->pos.x + (self->scale.x * self->sprite->frame_w)) > (bounds.w + bounds.x))
	{
		self->pos.x = (bounds.w + bounds.x) - (self->scale.x * self->sprite->frame_w);
	}
	if ((self->pos.y + (self->scale.y * self->sprite->frame_h)) > (bounds.h + bounds.y))
	{
		self->pos.y = (bounds.h + bounds.y) - (self->scale.y * self->sprite->frame_h);
	}
	if (self->pos.x < bounds.x)
	{
		self->pos.x = bounds.x;
	}
	if (self->pos.y < bounds.y)
	{
		self->pos.y = bounds.y;
	}
}