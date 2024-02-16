#include <SDL.h>
#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "drgn_camera.h"
#include "drgn_entity.h"
#include "drgn_player.h"
#include "drgn_army.h"
#include "drgn_unit.h"
#include "drgn_world.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    //Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,100,255,200);
    DRGN_Entity* player;
    DRGN_Entity* blueArmy;
    DRGN_Entity* redArmy;
    DRGN_Entity* greenArmy;
    DRGN_Entity* noArmy;
    DRGN_Entity* unit;
    int stats[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    DRGN_World* world;
    
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

    unit = drgn_unitNew(stats, NULL, "Test", DRGN_BLUE);

    blueArmy = drgn_armyNew(DRGN_BLUE, "blue");
    redArmy = drgn_armyNew(DRGN_RED, "red");
    greenArmy = drgn_armyNew(DRGN_GREEN, "green");
    noArmy = drgn_armyNew(DRGN_DEFAULT, "none");

    player = drgn_playerNew(blueArmy);

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        drgn_entitySystemThink();
        drgn_entitySystemUpdate();

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            //gf2d_sprite_draw_image(sprite,vector2d(0,0));

        drgn_worldDraw(world);
            drgn_entitySystemDraw();

            //UI elements last
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
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }

    //drgn_entityFree(player);
    drgn_entityCleanAll(NULL);
    drgn_worldFree(world);

    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
