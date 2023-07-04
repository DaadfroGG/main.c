
#include <boids.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define FPS 60
#define FRAME_DELAY (1000 / FPS)


void  Physics(player* myPlayer, Block blocks[], int numBlocks, Boid boids[])
{
    Vector2 prevPos = myPlayer->position;
    for (int j = 0; j < 19; j++)
    {
        boids[numBoids].prevPositions[j] = boids[numBoids].prevPositions[j + 1];
    }

    myPlayer->position.x += myPlayer->speed.x; // Update x position based on speed
    myPlayer->position.y += myPlayer->speed.y; // Update y position based on speed

    float playerLeft = myPlayer->position.x - myPlayer->size;
    float playerRight = myPlayer->position.x + myPlayer->size;
    float playerTop = myPlayer->position.y - myPlayer->size;
    float playerBottom = myPlayer->position.y + myPlayer->size;

    for (int i = 0; i < numBlocks; i++)
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
                // Handle collision with the block's top side
            }
            else if (collision.y < 0 && playerTop < floorBottom && prevPos.y >= floorBottom)
            {
                // printf("Collision: player below the block\n");
                myPlayer->speed.y = 0;
                myPlayer->position.y = blocks[i].position.y + blocks[i].size.y + myPlayer->size;
                // Handle collision with the block's bottom side
                collision.x = 0;
            }

            // Horizontal collision handling
            if (collision.x > 0 && playerRight > floorLeft && prevPos.x <= floorLeft)
            {
                // printf("Collision: player to the left of the block\n");
                myPlayer->speed.x = 0;
                myPlayer->position.x = blocks[i].position.x - myPlayer->size;
                // Handle collision with the block's left side
                collision.y = 0;
            }
            else if (collision.x < 0 && playerLeft < floorRight && prevPos.x >= floorRight)
            {
                // printf("Collision: player to the right of the block\n");
                myPlayer->speed.x = 0;
                myPlayer->position.x = blocks[i].position.x + blocks[i].size.x + myPlayer->size;
                // Handle collision with the block's right side
                collision.y = 0;
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

void HitBlockCheck(Boid boids[], HitBlock countBox[], int numBlocks)
{
    // Seed the random number generator
    srand(time(NULL));

    for (int j = 0; j < numBlocks; j++)
    {
        for (int i = 0; i < numBoids; i++)
        {
            // Calculate probability of teleporting out of the block based on permeability
            float teleportProbability = 1.0f - countBox[j].permuability;
            // Generate a random value between 0 and 1
            float randomValueTeleport = (float)rand() / RAND_MAX;
           
            if (randomValueTeleport > teleportProbability)
            {
                if (boids[i].x > countBox[j].position.x && boids[i].x < countBox[j].position.x + countBox[j].size.x &&
                    boids[i].y > countBox[j].position.y && boids[i].y < countBox[j].position.y + countBox[j].size.y)
                {
                    float absorbtionProbability = 1.0f - countBox[j].absorbtion;
                    float randomValueAbsorbtion = (float)rand() / RAND_MAX;
                    if (randomValueAbsorbtion > absorbtionProbability)
                    {
                        countBox[j].hit++;
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
    int numBlocks = 8;
    params p[1];
    Block blocks[numBlocks];
    HitBlock countBox[numBlocks];

    /*     blocks[4].position = (Vector2){ 800, 200 };
    blocks[4].size = (Vector2){ 200, 20 };
    blocks[4].rect = (Rectangle){ blocks[4].position.x, blocks[4].position.y, blocks[4].size.x, blocks[4].size.y };
    blocks[5].position = (Vector2){ 400, 300 };
    blocks[5].size = (Vector2){ 200, 20 };
    blocks[5].rect = (Rectangle){ blocks[5].position.x, blocks[5].position.y, blocks[5].size.x, blocks[5].size.y };
    blocks[6].position = (Vector2){ 100, 400 };
    blocks[6].size = (Vector2){ 200, 20 };
    blocks[6].rect = (Rectangle){ blocks[6  ].position.x, blocks[6].position.y, blocks[6].size.x, blocks[6].size.y }; */
    //the formula for passing from block coordinates to hitbox coordinates is: hitbox.x = block.x + block.width/2 - hitbox.width/2


    // Initialize boids and blocks
    InitBoids(boids);
    InitBlocks(blocks, countBox, numBlocks);
    // Set initial parameters for the player
    initPLayer(myPlayer, p, prevPos, teleportPos, teleport);
    // Game loop
    short quit = false;
    Uint32 frameStart;
    int frameTime;
    while (quit) {
        frameStart = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            }
        }
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        static int cameraX = 0;
        static int cameraY = 0;

        // Calculate target camera position based on player position
        int targetCameraX = myPlayer->position.x - screenWidth / 2;
        int targetCameraY = myPlayer->position.y - screenHeight / 2;

        // Apply "drag" effect to gradually move the camera towards the target position
        const float cameraDrag = 0.05; // Adjust the drag value for desired effect
        cameraX += (targetCameraX - cameraX) * cameraDrag;
        cameraY += (targetCameraY - cameraY) * cameraDrag;
        // Update game logic here...
        Movement(myPlayer);
        Control(&mousepos, teleport, teleportPos, myPlayer, p, cameraX, cameraY, boids);
        Update(boids, myPlayer->position, p[0]);
        // Perform physics and movement calculations for the player
        Physics(myPlayer, blocks, numBlocks, boids);
        HitBlockCheck(boids, countBox, numBlocks);
        // Render graphics here...
        Draw(myPlayer, boids, blocks, numBlocks, renderer, countBox, numBlocks, cameraX, cameraY);
        // Swap buffers
        SDL_RenderPresent(renderer);
        SDL_FlushEvent(SDL_MOUSEMOTION);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    // Cleanup and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}