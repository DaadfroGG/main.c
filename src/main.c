
#include <boids.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define FPS 60
#define FRAME_DELAY (1000 / FPS)


void  Physics(player* myPlayer, Block blocks[], HitBlock countBlock[], int numBlocks, Boid boids[])
{
    Vector2 prevPos = myPlayer->position;
    int jump_ticks = 20;
    for (int j = 0; j < 19; j++)
    {
        boids[numBoids].prevPositions[j] = boids[numBoids].prevPositions[j + 1];
    }
    myPlayer->position.x += myPlayer->speed.x; // Update x position based on speed
    myPlayer->position.y += myPlayer->speed.y; // Update y position based on speed

    // Calculate inertia based on previous position
    myPlayer->speed.x = myPlayer->position.x - prevPos.x + myPlayer->speed.x * 0.99;
    myPlayer->speed.y = myPlayer->position.y - prevPos.y;
    // update player's position
    myPlayer->position.x += myPlayer->speed.x * 3;
    myPlayer->position.y += myPlayer->speed.y;

    float playerLeft = myPlayer->position.x - myPlayer->size;
    float playerRight = myPlayer->position.x + myPlayer->size;
    float playerTop = myPlayer->position.y - myPlayer->size;
    float playerBottom = myPlayer->position.y + myPlayer->size;

    for (int i = 0; i < numBlocks; i++)
    {
        if (blocks[i].type == hitbox)
        {
            float floorLeft = blocks[i].position.x;
            float floorRight = blocks[i].position.x + blocks[i].size.x;
            float floorTop = blocks[i].position.y;
            float floorBottom = blocks[i].position.y + blocks[i].size.y;

            if (playerBottom > floorTop && playerTop < floorBottom && playerRight > floorLeft && playerLeft < floorRight)
            {
                Vector2 collision = subtractvect(myPlayer->position, prevPos);

                // Vertical collision handling
                if (collision.y > 0 && playerBottom > floorTop && prevPos.y <= floorTop)
                {
                    // printf("Collision: player above the block\n");
                    myPlayer->grounded = 1;
                    myPlayer->speed.y = 0;
                    myPlayer->position.y = blocks[i].position.y - myPlayer->size;
                    collision.x = 0;
                    myPlayer->grounded = jump_ticks;
                    // Handle collision with the block's top side
                }
                else if (collision.y < 0 && playerTop < floorBottom && prevPos.y >= floorBottom)
                {
                    // printf("Collision: player below the block\n");
                    myPlayer->speed.y = 0;
                    myPlayer->position.y = blocks[i].position.y + blocks[i].size.y + myPlayer->size;
                    // Handle collision with the block's bottom side
                    collision.x = 0;
                    // myPlayer->grounded = jump_ticks;
                }

                // Horizontal collision handling
                if (collision.x > 0 && playerRight > floorLeft && prevPos.x <= floorLeft)
                {
                    // printf("Collision: player to the left of the block\n");
                    myPlayer->speed.x = 0;
                    myPlayer->position.x = blocks[i].position.x - myPlayer->size;
                    // Handle collision with the block's left side
                    collision.y = 0;
                    myPlayer->grounded = jump_ticks;
                }
                else if (collision.x < 0 && playerLeft < floorRight && prevPos.x >= floorRight)
                {
                    // printf("Collision: player to the right of the block\n");
                    myPlayer->speed.x = 0;
                    myPlayer->position.x = blocks[i].position.x + blocks[i].size.x + myPlayer->size;
                    // Handle collision with the block's right side
                    collision.y = 0;
                    myPlayer->grounded = jump_ticks;
                }
            }
        } 
        else if (blocks[i].type == end)
        {
            //at the end, the player is teleported to the center of the end block
            float floorLeft = blocks[i].position.x;
            float floorRight = blocks[i].position.x + blocks[i].size.x;
            float floorTop = blocks[i].position.y;
            float floorBottom = blocks[i].position.y + blocks[i].size.y;
            if (playerBottom > floorTop && playerTop < floorBottom && playerRight > floorLeft && playerLeft < floorRight)
            {
                myPlayer->position.x = (floorLeft + floorRight) / 2;
                myPlayer->position.y = (floorTop + floorBottom) / 2;
                myPlayer->speed.x = 0;
                myPlayer->speed.y = 0;
                if (myPlayer->in_the_end == 0)
                    myPlayer->in_the_end = 1;
            }
            if (myPlayer->in_the_end == 2)
            {
                myPlayer->score = countBlock[i].hit;
            }
        }

    }
    boids[numBoids].prevPositions[19] = (Vector2){ boids[numBoids].x, boids[numBoids].y };
    boids[numBoids].x = myPlayer->position.x;
    boids[numBoids].y = myPlayer->position.y;
}


const int WINDOW_WIDTH = screenWidth;
const int WINDOW_HEIGHT = screenHeight;

#include <stdlib.h> // For rand() and srand() functions

void HitBlockCheck(Boid boids[], HitBlock countBox[], int numBlocks, player myPlayer[])
{
    // Seed the random number generator
    srand(time(NULL));
    int boidBlockGiven = 0;
    int boidBlockGive = 1;
    for (int j = 0; j < numBlocks; j++)
    {
        countBox[j].hit = 0;
        for (int i = 0; i < numBoids; i++)
        {
            if (boids[i].x > countBox[j].position.x && boids[i].x < countBox[j].position.x + countBox[j].size.x &&
                boids[i].y > countBox[j].position.y && boids[i].y < countBox[j].position.y + countBox[j].size.y)
            {
            countBox[j].hit++;

            // Calculate probability of teleporting out of the block based on permeability
            float teleportProbability = 1.0f - countBox[j].permuability;
            // Generate a random value between 0 and 1
            float randomValueTeleport = (float)rand() / RAND_MAX;

            if (randomValueTeleport > teleportProbability)
            {
                    float absorbtionProbability = 1.0f - countBox[j].absorbtion;
                    float randomValueAbsorbtion = (float)rand() / RAND_MAX;
                    if (randomValueAbsorbtion > absorbtionProbability)
                    {
                        float distanceToLeft = boids[i].x - countBox[j].position.x;
                        float distanceToRight = (countBox[j].position.x + countBox[j].size.x) - boids[i].x;
                        float distanceToTop = boids[i].y - countBox[j].position.y;
                        float distanceToBottom = (countBox[j].position.y + countBox[j].size.y) - boids[i].y;
                        
                        // Find the minimum distance among the four sides
                        float minDistance = fminf(distanceToLeft, fminf(distanceToRight, fminf(distanceToTop, distanceToBottom)));

                        // Teleport the boid to the closest side of the block
                        if (minDistance == distanceToLeft)
                            boids[i].x = countBox[j].position.x;
                        else if (minDistance == distanceToRight)
                            boids[i].x = countBox[j].position.x + countBox[j].size.x;
                        else if (minDistance == distanceToTop)
                            boids[i].y = countBox[j].position.y;
                        else
                            boids[i].y = countBox[j].position.y + countBox[j].size.y;
                    }
                    else
                        boids[i].alive = 0;

                }
        }
        if (countBox[j].type == end && myPlayer->in_the_end == 1)
            myPlayer->score = countBox[j].hit;

        if (countBox[j].type == boidbox)
        {
            if (myPlayer->position.x > countBox[j].position.x && myPlayer->position.x < countBox[j].position.x + countBox[j].size.x &&
                myPlayer->position.y > countBox[j].position.y && myPlayer->position.y < countBox[j].position.y + countBox[j].size.y && myPlayer->is_in_block == 0)
            {
                /* if (boids[i].alive == 0)
                    {
                    boids[i].alive = 1;
                    boids[i].x = myPlayer->position.x + rand() % 100 - 50; 
                    boids[i].y = myPlayer->position.y + rand() % 100 - 50;
                    for (int j = 0; j < 20; j++)
                    {
                        boids[i].prevPositions[j] = (Vector2){ boids[i].x, boids[i].y };
                    }
                    } */
                myPlayer->is_in_block = 1;
                for (int i = 0; i < numBoids && boidBlockGiven < (int)boidBlockGive; i++)
                {
                    if (boids[i].alive == 0)
                    {
                        boids[i].alive = 1;
                        boids[i].x = myPlayer->position.x + rand() % 100 - 50; 
                        boids[i].y = myPlayer->position.y + rand() % 100 - 50;
                        for (int j = 0; j < 20; j++)
                        {
                            boids[i].prevPositions[j] = (Vector2){ boids[i].x, boids[i].y };
                        }
                        boidBlockGiven++;
                    }
                }

            }
        }
        if (!(myPlayer->position.x > countBox[j].position.x && myPlayer->position.x < countBox[j].position.x + countBox[j].size.x &&
            myPlayer->position.y > countBox[j].position.y && myPlayer->position.y < countBox[j].position.y + countBox[j].size.y))
            myPlayer->is_in_block = 0;
        else
            myPlayer->is_in_block = 1;
    }
}

}


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
    // Vector2 cameraTether[15]; //curve defining the camera's authorised movement
    numBlocks[0] = 600; 
    //int zoom = 0.5;
    params p[1];
    Block blocks[numBlocks[0]];
    HitBlock countBox[numBlocks[0]];
    const int camera_switch = 200;
    int camera_switch_timer = 0;

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
    Vector2 pausecam = {(myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom,(myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom};
    static int cameraX;
    static int cameraY;    

    int bool_clement_camera = 0;

    cameraX = (myPlayer->position.x) - (WINDOW_WIDTH / 2) / zoom;
    cameraY = (myPlayer->position.y) - (WINDOW_HEIGHT / 2) / zoom;
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
            HitBlockCheck(boids, countBox, numBlocks[0], myPlayer);
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
            HitBlockCheck(boids, countBox, numBlocks[0], myPlayer);
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