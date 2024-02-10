#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "drgn_camera.h"
#include "drgn_world.h"

DRGN_World* drgn_worldNew(int width, int height)
{
	DRGN_World* self;
	self = gfc_allocate_array(sizeof(DRGN_World), 1);

	if (!self)
	{
		slog("No world could be created");
		return NULL;
	}

	if (!width || !height)
	{
		slog("Cannot create a world with %i width and %i height", width, height);
		return NULL;
	}

	self->width = width;
	self->height = height;
	self->tileMap = gfc_allocate_array(sizeof(Uint8), width * height);
	return (self);
}

void drgn_worldFree(DRGN_World* self)
{
	if (!self)
	{
		return;
	}

	gf2d_sprite_free(self->background);
	gf2d_sprite_free(self->tileSet);
	gf2d_sprite_free(self->tileLayer);
	free(self->tileMap);
	free(self);
}

void drgn_worldDraw(DRGN_World* self)
{
	Uint8 frame;
	int index;
	Vector2D pos;
	Vector2D offset;

	if (!self)
	{
		slog("No world to draw");
		return;
	}

	offset = drgn_cameraGetOffset();
	gf2d_sprite_draw_image(self->background, vector2d(0, 0));

	if (!self->tileSet)
	{
		slog("No tile set to draw");
		return;
	}

	gf2d_sprite_draw_image(self->tileLayer, offset);
}

DRGN_World* drgn_worldNewTest()
{
	DRGN_World* self;
	Vector2D size = drgn_cameraGetSize();
	int width = size.x / 64 , height = size.y/64;
	self = drgn_worldNew(width, height);

	if (!self)
	{
		return NULL;
	}

	self->background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
	self->tileSet = gf2d_sprite_load_all("images/newTile.png", 64, 64, 1, 1);

	for (int bogus = 0; bogus < width; bogus++)
	{
		self->tileMap[bogus] = 1;
		self->tileMap[bogus + ((height - 1) * width)] = 1;
	}

	for (int bogus = 0; bogus < height; bogus++)
	{
		self->tileMap[bogus * width] = 1;
		self->tileMap[bogus * width + (width - 1)] = 1;
	}

	drgn_worldTileLayerRender(self);
	return (self);
}

void drgn_worldTileLayerRender(DRGN_World* self)
{
	Vector2D pos;
	Uint32 frame;
	int index;
	Vector2D scale;

	if (!self)
	{
		return;
	}

	if (!self->tileSet)
	{
		slog("no tile set");
		return;
	}

	if (self->tileLayer)
	{
		gf2d_sprite_free(self->tileLayer);
	}

	self->tileLayer = gf2d_sprite_new();
	self->tileLayer->surface = gf2d_graphics_create_surface(self->width * self->tileSet->frame_w, self->height * self->tileSet->frame_h);
	self->tileLayer->frame_w = self->width * self->tileSet->frame_w;
	self->tileLayer->frame_h = self->height * self->tileSet->frame_h;

	if (!self->tileLayer->surface)
	{
		slog("No surface created");
		return;
	}

	for (int bogus = 0; bogus < self->height; bogus++)
	{
		for (int bogus2 = 0; bogus2 < self->width; bogus2++)
		{
			index = bogus2 + (bogus * self->width);

			if (self->tileMap[index] == 0)
			{
				continue;
			}

			pos.x = bogus2 * self->tileSet->frame_w;
			pos.y = bogus * self->tileSet->frame_h;
			frame = self->tileMap[index] - 1;
			scale = vector2d(4, 4);
			gf2d_sprite_draw_to_surface(self->tileSet, pos, NULL, NULL, frame, self->tileLayer->surface);
		}
	}

	self->tileLayer->texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(), self->tileLayer->surface);

	if (!self->tileLayer->texture)
	{
		slog("failed to create texture from surface");
		return;
	}
}

void drgn_worldCameraInit(DRGN_World* self)
{
	if (!self)
	{
		return;
	}

	if (!self->tileLayer || !self->tileLayer->surface)
	{
		slog("No tile layer for the world");
		return;
	}

	drgn_cameraSetBounds(gfc_rect(0, 0, self->tileLayer->surface->w, self->tileLayer->surface->h));
	drgn_cameraSetBind(1);
	drgn_cameraApplyBounds();
}