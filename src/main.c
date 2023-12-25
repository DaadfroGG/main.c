
#include <boids.h>
// typedef t_camera

#define MAX_TEXTURES 3
#define MAX_BLOCKS 600

typedef struct Game
{
    SDL_Window* window;
    // Renderer
    SDL_Renderer* renderer;
    // Distance from the center of the screen to the edge of the screen
    Vector2 center_distance;
    // Quit flag
    short quit;
    // Frame timing
    Uint32 frameStart;
    int frameTime;
    // Data for the events
    Vector2 mousepos;
    // Start position of the player (used for resetting)
    Vector2 start_pos;
    // Pointer to the player
    player player[1];
    int teleport[1];
    // Array storing all positions of boids in the level
    Boid boids[numBoids + 1];
    params p[1];
    // Array storing the number of blocks in the level
    int numBlocks[1];
    Block blocks[MAX_BLOCKS];
    HitBlock countBox[MAX_BLOCKS];
    // Camera position
    Vector2 camera;
    Vector2 pausecam;
    // Camera parameters
    double zoom;
    int pause;
    int bool_clement_camera;
    int camera_switch_timer;
}Game;


int main(void) {
    Game game;
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    // Create a window
    
    game.window = SDL_CreateWindow("main.c", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    // Create a renderer
    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_TARGETTEXTURE);
    //Initialize variables
    game.mousepos = (Vector2){ -100.0f, -100.0f };
    // Array storing all positions of blocks in the level
    game.numBlocks[0] = 600; 

    // Initialize boids and game.blocks
    InitBlocks(game.blocks, game.countBox, game.numBlocks, game.player);
    initPLayer(game.player, game.p, game.teleport);
    InitBoids(game.boids, game.player);
    game.start_pos = game.player->position;
    game.quit = false;
    game.zoom = 0.2; 
    game.pause = 0;  
    game.bool_clement_camera = 0;
    game.camera_switch_timer = 0;
    game.center_distance = (Vector2){(screenWidth * 0.5) / game.zoom,(screenHeight * 0.5) / game.zoom};
    game.pausecam = (Vector2){(game.player->position.x) - game.center_distance.x,(game.player->position.y) - game.center_distance.y};
    game.camera = (Vector2){(game.player->position.x) - game.center_distance.x, (game.player->position.y) - game.center_distance.y};
    game.player->in_the_end = 0;
    game.player->score = 0;
    game.player->position = game.start_pos;
    game.camera.x = (game.player->position.x) - game.center_distance.x;
    game.camera.y = (game.player->position.y) - game.center_distance.y;
    game.pausecam.x = (game.player->position.x) - game.center_distance.x;
    game.pausecam.y = (game.player->position.y) - game.center_distance.y;
    InitBoids(game.boids, game.player);

    game.pause = 0;




        while (game.quit) {

        game.frameStart = SDL_GetTicks();
        SDL_Event event;

        // Calculate target camera position based on player position and game.zoom
        game.center_distance = (Vector2){(screenWidth * 0.5) / game.zoom,(screenHeight * 0.5) / game.zoom};
        int targetCameraX = (game.player->position.x) - game.center_distance.x;
        int targetCameraY = (game.player->position.y) - game.center_distance.y;
        // Calculate game.zoom based on character speed
        
        // printf("%s\n", playerspeed);
        
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game.quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game.quit = true;
                }
                //handle game.p button for game.pause
                if (event.key.keysym.sym == SDLK_p) {
                    if (game.pause == 0)
                        game.pause = 1;
                    else
                        game.pause = 0;
                }
                if (game.player->in_the_end == 3 || event.key.keysym.sym == SDLK_r)
                {
                    game.player->in_the_end = 0;
                    game.player->score = 0;
                    initPLayer(game.player, game.p, game.teleport);
                    game.player->position = game.start_pos;
                    game.camera.x = targetCameraX;
                    game.camera.y = targetCameraY;
                    game.pausecam.x = game.camera.x;
                    game.pausecam.y = game.camera.y;
                    InitBoids(game.boids, game.player);
                    game.pause = 1;
                }
            }
            if (event.type == SDL_MOUSEWHEEL)
            {  
                if (event.wheel.y > 0) // scroll up
                {
                    //make the game.zoom less and less effective as it gets bigger (max game.zoom 1.0)
                    if (game.zoom < 1.0)
                        game.zoom += 0.01;
                    //recalculate camera position
                    targetCameraX = (game.player->position.x) - game.center_distance.x;
                    targetCameraY = (game.player->position.y) - game.center_distance.y;

                    game.camera.x += (targetCameraX - game.camera.x);
                    game.camera.y += (targetCameraY - game.camera.y);
                }
                else if (event.wheel.y < 0) // scroll down
                {
                    //make the game.zoom less and less effective as it gets bigger
                    if (game.zoom > 0.01)
                        game.zoom -= 0.01;

                    //recalculate camera position
                    targetCameraX = (game.player->position.x) - game.center_distance.x;
                    targetCameraY = (game.player->position.y) - game.center_distance.y;
                    game.camera.x += (targetCameraX - game.camera.x);
                    game.camera.y += (targetCameraY - game.camera.y);
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                    if (event.motion.state & SDL_BUTTON_RMASK) 
                    {
                        // Move the map
                        game.pausecam.x -= (float) event.motion.xrel / game.zoom;
                        game.pausecam.y -= (float) event.motion.yrel / game.zoom;
                    }
                }
        }
        // Clear the screen
        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);


        // Apply "drag" effect to gradually move the camera towards the target position
        const float cameraDrag = 0.05; // Adjust the drag value for desired effect
        const int camera_switch = 200;
        game.camera.x += (targetCameraX - game.camera.x) * cameraDrag;
        game.camera.y += (targetCameraY - game.camera.y) * cameraDrag;
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C]) //make camera position snap to the player
        {
            if ((int)game.frameStart - game.camera_switch_timer > camera_switch)
            {
                if (game.player->camera_type == 1)
                    game.player->camera_type = 0;
                else
                    game.player->camera_type = 1;

                game.camera_switch_timer = game.frameStart; // Reset the timer
            }
        }
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_KP_PLUS])
        {
            //make the game.zoom less and less effective as it gets bigger (max game.zoom 1.0)
            if (game.zoom < 1.0)
                game.zoom += 0.01;
            //recalculate camera position
            targetCameraX = (game.player->position.x) - game.center_distance.x;
            targetCameraY = (game.player->position.y) - game.center_distance.y;
            
            game.camera.x += (targetCameraX - game.camera.x);
            game.camera.y += (targetCameraY - game.camera.y);
            
        }
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_KP_MINUS])
        {
            //make the game.zoom less and less effective as it gets bigger
            if (game.zoom > 0.01)
                game.zoom -= 0.01;

            //recalculate camera position
            targetCameraX = (game.player->position.x) - game.center_distance.x;
            targetCameraY = (game.player->position.y) - game.center_distance.y;
            game.camera.x += (targetCameraX - game.camera.x);
            game.camera.y += (targetCameraY - game.camera.y);
        }
        if (game.pause)
        {
            game.camera.x = game.pausecam.x;
            game.camera.y = game.pausecam.y;
        }
        else
        {
            game.pausecam = (Vector2){(game.player->position.x) - game.center_distance.x,(game.player->position.y) - game.center_distance.y};
        }
        // Update game logic here...
        if (game.pause == 0)
        {
            if (game.player->in_the_end == 0)
            {
                game.player->end_time = SDL_GetTicks();
                Movement(game.player);
                Control(&game.mousepos, game.teleport, game.player, game.p, game.camera.x, game.camera.y, game.boids, game.zoom, game.blocks, game.numBlocks[0]);
            }
            Update(game.boids, game.player->position, game.p[0], *game.teleport);
            // Resize the map according to the game.zoom to make the player bigger

            Physics(game.player, game.blocks,game.countBox , game.numBlocks[0], game.boids);
            BoidCollision(game.boids, game.countBox, game.numBlocks[0], game.player);
            // Render graphics here...
        }
        else
        {
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT] && !game.bool_clement_camera)
            {
                game.bool_clement_camera = 1;
            if (game.player->in_the_end == 0)
            {
                game.player->end_time = SDL_GetTicks();
                Movement(game.player);
                Control(&game.mousepos, game.teleport, game.player, game.p, game.camera.x, game.camera.y, game.boids, game.zoom, game.blocks, game.numBlocks[0]);
            }
            Update(game.boids, game.player->position, game.p[0], *game.teleport);
            // Resize the map according to the game.zoom to make the player bigger
            Physics(game.player, game.blocks,game.countBox , game.numBlocks[0], game.boids);
            BoidCollision(game.boids, game.countBox, game.numBlocks[0], game.player);
            }
            else
                game.bool_clement_camera = 0;
        }
        if (game.player->camera_type)
        {
            game.camera.x = (game.player->position.x) - game.center_distance.x;
            game.camera.y = (game.player->position.y) - game.center_distance.y;
        }
        Draw(game.player, game.boids, game.blocks, game.numBlocks[0], game.renderer, game.countBox, game.numBlocks[0], game.camera.x, game.camera.y, game.p, game.zoom, game.pause);
        convexHull(game.boids, game.player);
        // Swap buffers
        SDL_RenderPresent(game.renderer);
        SDL_FlushEvent(SDL_MOUSEMOTION);
        game.frameTime = SDL_GetTicks() - game.frameStart;
        if (game.frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - game.frameTime);
        }
        //start timer if player is in the end
        if (game.player->in_the_end == 1)
        {
            game.player->timer = SDL_GetTicks();
            game.player->in_the_end = 2;
        }
        //if player is in the end and 5 seconds have passed, the score is shown
        if (game.player->in_the_end == 2)
        {
            if (SDL_GetTicks() - game.player->timer > 5000)
            {
                game.player->in_the_end = 3;

            }
        }
    }
    // Cleanup and quit SDL
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    SDL_Quit();


    return 0;
}