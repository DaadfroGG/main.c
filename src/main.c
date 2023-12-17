
#include <boids.h>


const int WINDOW_WIDTH = screenWidth;
const int WINDOW_HEIGHT = screenHeight;


// typedef t_camera

int main(void) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    // Create a window
    SDL_Window* window = SDL_CreateWindow("main.c", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);



    //Initialize variables
    Vector2 mousepos = { -100.0f, -100.0f };
    Boid boids[numBoids + 1];
    player myPlayer[1];
    Vector2 prevPos[1];
    Vector2 teleportPos[1];
    int teleport[1];
    int numBlocks[1];
    params p[1];
    // Vector2 cameraTether[15]; //curve defining the camera's authorised movement


    // Array storing all positions of blocks in the level
    numBlocks[0] = 600; 
    Block blocks[numBlocks[0]];
    HitBlock countBox[numBlocks[0]];



    // Initialize boids and blocks
    InitBlocks(blocks, countBox, numBlocks, myPlayer);
    initPLayer(myPlayer, p, prevPos, teleportPos, teleport);
    InitBoids(boids, myPlayer);
    Vector2 start_pos = myPlayer->position;

    // Set initial parameters for the player
    // Game loop
    short quit = false;
    Uint32 frameStart;
    int frameTime;
    double zoom = 0.8; 
    int pause = 0;
    static int cameraX;
    static int cameraY;    
    int bool_clement_camera = 0;
    int camera_switch_timer = 0;
    const int camera_switch = 200;
    Vector2 pausecam = {(myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom,(myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom};
    cameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
    cameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;




    myPlayer->in_the_end = 0;
    myPlayer->score = 0;
    initPLayer(myPlayer, p, prevPos, teleportPos, teleport);
    myPlayer->position = start_pos;
    cameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
    cameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
    pausecam.x = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
    pausecam.y = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
    InitBoids(boids, myPlayer);
    pause = 0;



        while (quit) {

        frameStart = SDL_GetTicks();
        SDL_Event event;

        // Calculate target camera position based on player position and zoom
        int targetCameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
        int targetCameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                //handle p button for pause
                if (event.key.keysym.sym == SDLK_p) {
                    if (pause == 0)
                        pause = 1;
                    else
                        pause = 0;
                }
                if (myPlayer->in_the_end == 3 || event.key.keysym.sym == SDLK_r)
                {
                    myPlayer->in_the_end = 0;
                    myPlayer->score = 0;
                    initPLayer(myPlayer, p, prevPos, teleportPos, teleport);
                    myPlayer->position = start_pos;
                    cameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
                    cameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
                    pausecam.x = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
                    pausecam.y = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
                    InitBoids(boids, myPlayer);
                    pause = 1;
                }
            }
            if (event.type == SDL_MOUSEWHEEL)
            {  
                if (event.wheel.y > 0) // scroll up
                {
                    //make the zoom less and less effective as it gets bigger (max zoom 1.0)
                    if (zoom < 1.0)
                        zoom += 0.01;
                    //recalculate camera position
                    targetCameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
                    targetCameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
                    
                    cameraX += (targetCameraX - cameraX);
                    cameraY += (targetCameraY - cameraY);
                }
                else if (event.wheel.y < 0) // scroll down
                {
                    //make the zoom less and less effective as it gets bigger
                    if (zoom > 0.01)
                        zoom -= 0.01;

                    //recalculate camera position
                    targetCameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
                    targetCameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
                    cameraX += (targetCameraX - cameraX);
                    cameraY += (targetCameraY - cameraY);
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                    if (event.motion.state & SDL_BUTTON_RMASK) 
                    {
                        // Move the map
                        pausecam.x -= (float) event.motion.xrel / zoom;
                        pausecam.y -= (float) event.motion.yrel / zoom;
                    }
                }
        }
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // Apply "drag" effect to gradually move the camera towards the target position
        const float cameraDrag = 0.05; // Adjust the drag value for desired effect
        cameraX += (targetCameraX - cameraX) * cameraDrag;
        cameraY += (targetCameraY - cameraY) * cameraDrag;
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C]) //make camera position snap to the player
        {
            if ((int)frameStart - camera_switch_timer > camera_switch)
            {
                if (myPlayer->camera_type == 1)
                    myPlayer->camera_type = 0;
                else
                    myPlayer->camera_type = 1;

                camera_switch_timer = frameStart; // Reset the timer
            }
        }
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_KP_PLUS])
        {
            //make the zoom less and less effective as it gets bigger (max zoom 1.0)
            if (zoom < 1.0)
                zoom += 0.01;
            //recalculate camera position
            targetCameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
            targetCameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
            
            cameraX += (targetCameraX - cameraX);
            cameraY += (targetCameraY - cameraY);
            
        }
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_KP_MINUS])
        {
            //make the zoom less and less effective as it gets bigger
            if (zoom > 0.01)
                zoom -= 0.01;

            //recalculate camera position
            targetCameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
            targetCameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
            cameraX += (targetCameraX - cameraX);
            cameraY += (targetCameraY - cameraY);
        }
        if (pause)
        {
            cameraX = pausecam.x;
            cameraY = pausecam.y;
        }
        else
        {
            pausecam = (Vector2){(myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom,(myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom};
        }
        // Update game logic here...
        if (pause == 0)
        {
            if (myPlayer->in_the_end == 0)
            {
                myPlayer->end_time = SDL_GetTicks();
                Movement(myPlayer);
                Control(&mousepos, teleport, teleportPos, myPlayer, p, cameraX, cameraY, boids, zoom, blocks, numBlocks[0]);
            }
            Update(boids, myPlayer->position, p[0], *teleport);
            // Resize the map according to the zoom to make the player bigger

            Physics(myPlayer, blocks,countBox , numBlocks[0], boids);
            BoidCollision(boids, countBox, numBlocks[0], myPlayer);
            // Render graphics here...
        }
        else
        {
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT] && !bool_clement_camera)
            {
                bool_clement_camera = 1;
            if (myPlayer->in_the_end == 0)
            {
                myPlayer->end_time = SDL_GetTicks();
                Movement(myPlayer);
                Control(&mousepos, teleport, teleportPos, myPlayer, p, cameraX, cameraY, boids, zoom, blocks, numBlocks[0]);
            }
            Update(boids, myPlayer->position, p[0], *teleport);
            // Resize the map according to the zoom to make the player bigger
            Physics(myPlayer, blocks,countBox , numBlocks[0], boids);
            BoidCollision(boids, countBox, numBlocks[0], myPlayer);
            }
            else
                bool_clement_camera = 0;
        }
        if (myPlayer->camera_type)
        {
            cameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
            cameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
        }
        Draw(myPlayer, boids, blocks, numBlocks[0], renderer, countBox, numBlocks[0], cameraX, cameraY, p, zoom, pause);
        convexHull(boids, myPlayer);
        // Swap buffers
        SDL_RenderPresent(renderer);
        SDL_FlushEvent(SDL_MOUSEMOTION);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
        //start timer if player is in the end
        if (myPlayer->in_the_end == 1)
        {
            myPlayer->timer = SDL_GetTicks();
            myPlayer->in_the_end = 2;
        }
        //if player is in the end and 5 seconds have passed, the score is shown
        if (myPlayer->in_the_end == 2)
        {
            if (SDL_GetTicks() - myPlayer->timer > 5000)
            {
                myPlayer->in_the_end = 3;

            }
        }
    }
    // Cleanup and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}