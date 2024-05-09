#include "simple_logger.h"
#include "drgn_windels.h"
#include "drgn_unit.h"

DRGN_Windel* drgn_windelNew(SJson* object, Vector2D parentPos)
{
	DRGN_Windel* windel;
	int check;
	float red, green, blue, alpha;

	windel = gfc_allocate_array(sizeof(DRGN_Windel), 1);

	if (!object)
	{
		slog("no no json object given for the windel");
		return NULL;
	}

	windel->name = sj_object_get_value_as_string(object, "name");
	check = sj_object_get_value_as_float(object, "posX", &windel->pos.x);

	if (!check)
	{
		windel->pos.x = 0;
	}

	check = sj_object_get_value_as_float(object, "posY", &windel->pos.y);

	if (!check)
	{
		windel->pos.y = 0;
	}

	vector2d_add(windel->pos, windel->pos, parentPos);

	check = sj_object_get_value_as_float(object, "scaleX", &windel->scale.x);

	if (!check)
	{
		windel->scale.x = 1;
	}

	check = sj_object_get_value_as_float(object, "scaleY", &windel->scale.y);

	if (!check)
	{
		windel->scale.y = 1;
	}

	check = sj_object_get_value_as_float(object, "colorR", &red);

	if (!check)
	{
		return (windel);
	}

	check = sj_object_get_value_as_float(object, "colorG", &green);

	if (!check)
	{
		return (windel);
	}

	check = sj_object_get_value_as_float(object, "colorB", &blue);

	if (!check)
	{
		return (windel);
	}

	check = sj_object_get_value_as_float(object, "colorA", &alpha);

	if (!check)
	{
		return (windel);
	}

	windel->color = gfc_color8(red, green, blue, alpha);
	windel->selectable = 0;

	return (windel);
}

DRGN_Windel* drgn_windelAdd(const char* name, Vector2D pos, Vector2D parentPos, Vector2D scale, Color* color)
{
	DRGN_Windel* windel;

	if (!name)
	{
		slog("no name or text given");
		return NULL;
	}

	windel = gfc_allocate_array(sizeof(DRGN_Windel), 1);

	if (!windel)
	{
		slog("no windel could be created");
		return NULL;
	}

	windel->name = name;
	vector2d_add(windel->pos, pos, parentPos);
	vector2d_copy(windel->scale, scale);

	if (color)
	{
		windel->color = *color;
	}
}

void drgn_windelFree(DRGN_Windel* windel)
{
	slog("freeing element %s", windel->name);
	
	if (!windel)
	{
		slog("no element to free");
		return;
	}

	if (windel->free)
	{
		windel->free(windel);
	}

	free(windel);
}

void drgn_windelUpdate(DRGN_Windel* windel)
{
	if (!windel)
	{
		return;
	}

	if (windel->update)
	{
		windel->update(windel);
	}
}

void drgn_windelDraw(DRGN_Windel* windel)
{
	if (!windel)
	{
		return;
	}

	if (windel->draw)
	{
		windel->draw(windel);
	}
}

DRGN_Windel* drgn_windelTextNew(SJson* object, Vector2D parentPos)
{
	DRGN_Windel* windel;
	DRGN_WindelText* textBox;
	const char* text;

	if (!object)
	{
		slog("no json object given");
		return NULL;
	}

	windel = drgn_windelNew(object, parentPos);

	if (!windel)
	{
		slog("windel was not created successfully");
		return NULL;
	}

	windel->free = drgn_windelTextFree;
	windel->update = drgn_windelTextUpdate;
	windel->draw = drgn_windelTextDraw;

	textBox = gfc_allocate_array(sizeof(DRGN_WindelText), 1);

	if (!textBox)
	{
		slog("text box windel could not be created");
		drgn_windelFree(windel);
		return NULL;
	}

	text = sj_object_get_value_as_string(object, "text");

	if (!text)
	{
		slog("no text given");
		drgn_windelFree(windel);
		return NULL;
	}

	textBox->text = gfc_allocate_array(sizeof(char), strlen(text) + 1);
	strcpy(textBox->text, text);
	sj_object_get_value_as_int(object, "size", &textBox->style);
	windel->data = textBox;

	slog("created text with text %s", textBox->text);
	return (windel);
}

DRGN_Windel* drgn_windelTextAdd(const char* name, Vector2D pos, Vector2D parentPos, Vector2D scale, Color* color, const char* text, DRGN_FontStyles style)
{
	DRGN_Windel* windel;
	DRGN_WindelText* textBox;

	windel = drgn_windelAdd(name, pos, parentPos, scale, color);
	windel->free = drgn_windelTextFree;
	windel->update = drgn_windelTextUpdate;
	windel->draw = drgn_windelTextDraw;
	textBox = gfc_allocate_array(sizeof(DRGN_WindelText), 1);

	if (!textBox)
	{
		slog("text box could not be created");
		drgn_windelFree(windel);
		return NULL;
	}

	textBox->text = gfc_allocate_array(sizeof(char), strlen(text) + 1);
	strcpy(textBox->text, text);
	textBox->style = style;
	windel->data = textBox;
	return (windel);
}

void drgn_windelTextFree(DRGN_Windel* windel)
{
	DRGN_WindelText* text;

	if (!windel || !windel->data)
	{
		return;
	}

	text = (DRGN_WindelText*)windel->data;
	free(text->text);
	free(text);
}

void drgn_windelTextUpdate(DRGN_Windel* windel)
{
	if (!windel)
	{
		return;
	}
}

void drgn_windelTextDraw(DRGN_Windel* windel)
{
	DRGN_WindelText* text;

	if (!windel || !windel->data)
	{
		return;
	}

	text = (DRGN_WindelText*)windel->data;
	//slog("scale: %f, %f", windel->scale.x, windel->scale.y);
	drgn_fontDraw(text->text, text->style, windel->color, windel->pos, &windel->scale);
}

void drgn_windelTextChangeText(DRGN_Windel* windel, const char* newText)
{
	DRGN_WindelText* text;

	if (!windel || !windel->data || !newText)
	{
		return;
	}

	text = (DRGN_WindelText*)windel->data;

	if (!text)
	{
		return;
	}

	text->text = gfc_allocate_array(strlen(newText) + 1, 1);
	strcpy(text->text, newText);
	//slog("%s", newText);
}

DRGN_Windel* drgn_windelSpriteNew(SJson* object, Vector2D parentPos)
{
	DRGN_Windel* windel;
	DRGN_WindelSprite* image;
	const char* sprite;

	windel = drgn_windelNew(object, parentPos);

	if (!windel)
	{
		slog("Could not create windel");
		return NULL;
	}

	windel->free = drgn_windelSpriteFree;
	windel->update = drgn_windelSpriteUpdate;
	windel->draw = drgn_windelSpriteDraw;
	image = gfc_allocate_array(sizeof(DRGN_WindelSprite), 1);

	if (!image)
	{
		slog("Could not create windel's sprite data");
		drgn_windelFree(windel);
		return NULL;
	}

	sprite = sj_object_get_value_as_string(object, "sprite");

	if (!sprite)
	{
		slog("No sprite to populate the windel");
		drgn_windelFree(windel);
		return NULL;
	}

	image->sprite = gf2d_sprite_load_image(sprite);
	sj_object_get_value_as_uint8(object, "useColor", &image->useColor);
	windel->data = image;

	return (windel);
}

void drgn_windelSpriteFree(DRGN_Windel* windel)
{
	DRGN_WindelSprite* image;

	if (!windel || !windel->data)
	{
		return;
	}

	image = (DRGN_WindelSprite*)windel->data;

	gf2d_sprite_free(image->sprite);
	free(image);
}

void drgn_windelSpriteUpdate(DRGN_Windel* windel)
{
	if (!windel || !windel->data)
	{
		return;
	}
}

void drgn_windelSpriteDraw(DRGN_Windel* windel)
{
	DRGN_WindelSprite* image;

	if (!windel || !windel->data)
	{
		return;
	}

	image = (DRGN_WindelSprite*)windel->data;

	if (image->useColor)
	{
		gf2d_sprite_render(image->sprite, windel->pos, &windel->scale, NULL, NULL, NULL, &windel->color, NULL, (Uint32)image->frame);
	}
	else
	{
		gf2d_sprite_render(image->sprite, windel->pos, &windel->scale, NULL, NULL, NULL, NULL, NULL, (Uint32)image->frame);
	}
}

Uint32 drgn_windelSpriteGetWidth(DRGN_Windel* windel)
{
	DRGN_WindelSprite* sprite;

	if (!windel || !windel->data)
	{
		return NULL;
	}

	sprite = (DRGN_WindelSprite*)windel->data;

	if (!sprite)
	{
		return NULL;
	}

	return (sprite->sprite->frame_w);
}

Uint32 drgn_windelSpriteGetHeight(DRGN_Windel* windel)
{
	DRGN_WindelSprite* sprite;

	if (!windel || !windel->data)
	{
		return NULL;
	}

	sprite = (DRGN_WindelSprite*)windel->data;

	if (!sprite)
	{
		return NULL;
	}

	return (sprite->sprite->frame_h);
}

DRGN_Windel* drgn_windelButtonNew(SJson* object, Vector2D parentPos, DRGN_ButtonAction action, DRGN_Entity* parent)
{
	DRGN_Windel* windel;
	DRGN_WindelButton* button;

	windel = drgn_windelNew(object, parentPos);

	if (!windel)
	{
		slog("Could not create windel");
		return NULL;
	}

	windel->free = drgn_windelButtonFree;
	windel->update = drgn_windelButtonUpdate;
	windel->draw = drgn_windelButtonDraw;

	button = gfc_allocate_array(sizeof(DRGN_WindelButton), 1);

	if (!button)
	{
		slog("Could not create button data for windel");
		drgn_windelFree(windel);
		return NULL;
	}

	button->pushed = 0;
	button->action = action;
	button->parent = parent;
	windel->data = button;

	return (windel);
}

DRGN_Windel* drgn_windelButtonAdd(const char* name, Vector2D pos, Vector2D parentPos, Vector2D scale, Color* color, DRGN_ButtonAction action, DRGN_Entity* parent)
{
	DRGN_Windel* windel;
	DRGN_WindelButton* button;

	windel = drgn_windelAdd(name, pos, parentPos, scale, color);
	windel->free = drgn_windelButtonFree;
	windel->update = drgn_windelButtonUpdate;
	windel->draw = drgn_windelButtonDraw;
	button = gfc_allocate_array(sizeof(DRGN_WindelButton*), 1);
	button->action = action;
	button->parent = parent;
	windel->data = button;
	return (windel);
}

void drgn_windelButtonFree(DRGN_Windel* windel)
{
	DRGN_WindelButton* button;

	if (!windel || !windel->data)
	{
		return;
	}

	button = (DRGN_WindelButton*)windel->data;
	slog("about to free %s", windel->name);
}

void drgn_windelButtonUpdate(DRGN_Windel* windel)
{
	DRGN_WindelButton* button;

	if (!windel || !windel->data)
	{
		return;
	}

	button = (DRGN_WindelButton*)windel->data;
	//slog("Button: x: %f, y: %f", windel->pos.x, windel->pos.y);

	if (button->pushed)
	{
		//Do push action here
		
		button->pushed = 0;
		//slog("pushed");
		drgn_windelButtonCompleteAction(button);
		//slog("%i", button->action);
		return;
	}
}

void drgn_windelButtonDraw(DRGN_Windel* windel)
{
	if (!windel || !windel->data)
	{
		return;
	}
}

void drgn_windelButtonCompleteAction(DRGN_WindelButton* button)
{
	DRGN_Unit* unit;

	if (!button || !button->parent)
	{
		return;
	}

	unit = (DRGN_Unit*)button->parent->data;

	if (!unit)
	{
		return;
	}

	switch (button->action)
	{
		case DRGN_BUTTON_NONE:
			return;
		case DRGN_BUTTON_SEIZE:
			unit->currentAction = DRGN_SEIZE;
			break;
		case DRGN_BUTTON_TALK:
			unit->currentAction = DRGN_TALK;
			break;
		case DRGN_BUTTON_MELEE_ATTACK:
			unit->currentAction = DRGN_MELEE_ATTACK;
			break;
		case DRGN_BUTTON_RANGED_ATTACK:
			unit->currentAction = DRGN_RANGED_ATTACK;
			break;
		case DRGN_BUTTON_MAGIC_ATTACK:
			unit->currentAction = DRGN_MAGIC_ATTACK;
			break;
		case DRGN_BUTTON_HEAL:
			unit->currentAction = DRGN_HEAL;
			break;
		case DRGN_BUTTON_ITEM:
			unit->currentAction = DRGN_ITEM;
			break;
		case DRGN_BUTTON_TRADE:
			unit->currentAction = DRGN_TRADE;
			break;
		case DRGN_BUTTON_RESCUE:
			unit->currentAction = DRGN_RESCUE;
			break;
		case DRGN_BUTTON_TRANSFER:
			unit->currentAction = DRGN_TRANSFER;
			break;
		case DRGN_BUTTON_DROP:
			unit->currentAction = DRGN_DROP;
			break;
		case DRGN_BUTTON_WAIT:
			unit->currentAction = DRGN_WAIT;
			break;
		case DRGN_BUTTON_ITEM_SHOP:
			unit->currentAction = DRGN_ITEM_SHOP;
			break;
		case DRGN_BUTTON_ARMORY:
			unit->currentAction = DRGN_ARMORY;
			break;
		case DRGN_BUTTON_BUY:
			//slog("buying items");
			unit->currentAction = DRGN_BUY;
			break;
		case DRGN_BUTTON_SELL:
			unit->currentAction = DRGN_SELL;
			break;
		case DRGN_BUTTON_PURCHASE:
			unit->currentAction = DRGN_PURCHASE;
			break;
		default:
			return;
	}

	//slog("%i action", unit->currentAction);
	drgn_unitMenu(button->parent);
}