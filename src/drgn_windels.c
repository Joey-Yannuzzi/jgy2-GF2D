#include "simple_logger.h"
#include "drgn_windels.h"

DRGN_Windel* drgn_windelNew(const char* name, Vector2D pos, Vector2D* scale, Color* color)
{
	DRGN_Windel* windel;

	windel = gfc_allocate_array(sizeof(DRGN_Windel), 1);

	if (!name)
	{
		slog("no name given for the windel");
		return NULL;
	}

	windel->name = name;
	windel->pos = pos;
	
	if (scale)
	{
		windel->scale.x = scale->x;
		windel->scale.y = scale->y;
	}
	else
	{
		vector2d_copy(windel->scale, vector2d(1, 1));
	}

	if (color)
	{
		windel->color = *color;
	}
	else
	{
		windel->color = GFC_COLOR_BLACK;
	}

	return (windel);
}

void drgn_windelFree(DRGN_Windel* windel)
{
	if (!windel)
	{
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

DRGN_Windel* drgn_windelTextNew(const char* name, Vector2D pos, Vector2D* scale, Color* color, char* text, DRGN_FontStyles style)
{
	DRGN_Windel* windel;
	DRGN_WindelText* textBox;

	windel = drgn_windelNew(name, pos, scale, color);

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

	if (!text)
	{
		slog("could not get text for windel");
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
	drgn_fontDraw(text->text, text->style, windel->color, windel->pos, NULL);
}

DRGN_Windel* drgn_windelSpriteNew(const char* name, Vector2D pos, Vector2D* scale, Color* color, Sprite* sprite)
{
	DRGN_Windel* windel;
	DRGN_WindelSprite* image;

	windel = drgn_windelNew(name, pos, scale, color);

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

	if (!sprite)
	{
		slog("No sprite to populate the windel");
		drgn_windelFree(windel);
		return NULL;
	}

	image->sprite = sprite;

	if (color)
	{
		image->useColor = 1;
	}

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

DRGN_Windel* drgn_windelButtonNew(const char* name, Vector2D pos, Vector2D* scale, Color* color)
{
	DRGN_Windel* windel;
	DRGN_WindelButton* button;

	windel = drgn_windelNew(name, pos, scale, color);

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
	free(button);
}

void drgn_windelButtonUpdate(DRGN_Windel* windel)
{
	DRGN_WindelButton* button;

	if (!windel || !windel->data)
	{
		return;
	}

	button = (DRGN_WindelButton*)windel->data;

	if (button->pushed)
	{
		//Do push action here
		button->pushed = 0;
	}
}

void drgn_windelButtonDraw(DRGN_Windel* windel)
{
	if (!windel || !windel->data)
	{
		return;
	}
}