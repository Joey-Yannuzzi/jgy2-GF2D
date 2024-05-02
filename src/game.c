#include <SDL.h>
#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "drgn_font.h"
#include "drgn_camera.h"
#include "drgn_entity.h"
#include "drgn_player.h"
#include "drgn_army.h"
#include "drgn_unit.h"
#include "drgn_world.h"
#include "drgn_inventory.h"
#include "drgn_terrain.h"
#include "drgn_window.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Uint32* mouseInput;
    //Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,100,255,200);
    DRGN_Entity* player;
    DRGN_World* world;
    DRGN_Window* window;
    SDL_Event event;
    //DRGN_Entity* window;
    //Color color = gfc_color8(255, 0, 0, 100);
    //Vector2D vect = vector2d(1, 1);
    
    /*program initializtion*/
    init_logger("gf2d.log",0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "Dragoon",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);

    drgn_entitySystemInit(1024);
    drgn_inventoryFileInit("defs/inventory/drgn_items.json");
    drgn_unitFileInit("defs/drgn_unit.json");
    drgn_terrainFileInit("defs/drgn_terrain.json");
    drgn_windowFileInit("defs/drgn_windows.json");
    drgn_fontInit();
    drgn_windowManagerNew(1024);
    SDL_ShowCursor(SDL_DISABLE);

    drgn_cameraSetSize(vector2d(1200, 720));
    
    /*demo setup*/
    //sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);

    //slog("%i", (*(DRGN_Player*)player->data).test);

    world = drgn_worldLoad("defs/maps/drgn_test.json");
    drgn_worldCameraInit(world);
    /*
    * 
    * Create units first
    * Then create armies and populate them with units
    * Finally, create the player, and populate with player's army
    */

    //window = drgn_windowNew("hello world", "images/windows/testWindow.png", 128, 64, vector2d(64, 64));
    player = drgn_playerNew();
    //sprite = gf2d_sprite_load_all("images/tiles/move.png", 64, 64, 1, 0);
    //window = drgn_windowNew("commandWindow");
    window = NULL;

    slog("Begining game");
    /*main game loop*/
    while(!done)
    {
        drgn_fontCacheListFree();
        //slog("here");
        //slog("Unit named %s on field", ((DRGN_Unit*)unit->data)->name);
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //SDL_PollEvent(&event);
        /*update things here*/
        mouseInput = SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        drgn_entitySystemThink();
        drgn_entitySystemUpdate();
        drgn_windowUpdateAll();

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first

        drgn_worldDraw(world);
        //gf2d_sprite_render(sprite, vector2d(0, 0), &vect, NULL, NULL, NULL, &color, NULL, 0);
            drgn_entitySystemDraw();

            //drgn_fontDraw("Hello\nWorld", DRGN_LARGE_FONT, GFC_COLOR_BLACK, player->pos);
            //UI elements last

            drgn_windowDrawAll();
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);

        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame
        //slog("Player pos: %f, %f", player->pos.x, player->pos.y);
        //slog("Unit pos: %f, %f", unit->pos.x, unit->pos.y);
        
        if (keys[SDL_SCANCODE_1] && !window)
        {
            window = drgn_windowNew("pauseMenu", DRGN_BUTTON_NONE, NULL);
            drgn_windowAssignActionByName(window, "resumeButton", DRGN_BUTTON_RESUME);
            drgn_windowAssignActionByName(window, "quitButton", DRGN_BUTTON_QUIT);
            drgn_entitySetAllInactive();
        }

        /*if (mouseInput[SDL_MOUSEBUTTONDOWN])
        {
            slog("click!");
        }*/
        
        // exit condition
        if (keys[SDL_SCANCODE_ESCAPE])
        {
            done = 1;
        }
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }

    //drgn_entityFree(player);
    slog("begin cleaning");
    drgn_entityCleanAll(NULL);
    slog("cleaned entities successfully");
    drgn_worldFree(world);
    slog("cleaned world successfully");
    drgn_inventoryClose();
    slog("cleaned inventory successfully");
    drgn_windowFreeAll();
    slog("freed windows successfully");

    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
