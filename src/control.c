#include <boids.h>

double gravity = 0.5;

void Movement(player* myPlayer)
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    int move_speed = 30;
    myPlayer->speed.y += gravity; // Apply gravity
    if (keyboardState[SDL_SCANCODE_D])
    {
        myPlayer->speed.x = move_speed;
    }
    else if (keyboardState[SDL_SCANCODE_A])
    {
        myPlayer->speed.x = -move_speed;
    }
    else
    {
        myPlayer->speed.x = 0; // No lateral movement if no key is pressed
    }

    // Handle jumping
    if (keyboardState[SDL_SCANCODE_W])
    {
        myPlayer->speed.y = -move_speed;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        myPlayer->speed.y = move_speed;
    }
}

void Control(Vector2* mousepos, int* teleport, Vector2* teleportPos, player* myPlayer, params* p, int cameraX, int cameraY, Boid boids[])
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    mousepos->x = mouseX;
    mousepos->y = mouseY;
    
    // Adjust the click position based on the camera offset
    mousepos->x += cameraX;
    mousepos->y += cameraY;
    
    static int timer = 0;
    static int deactivation_timer = 0;
    int time_limit = 300;
    int delay_duration = 100; // Delay duration 
    int currentTime = SDL_GetTicks();
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_R] == SDL_PRESSED)
    {
        for (int i = 0; i < numBoids; i++)
        {
            boids[i].alive = 1;
        }
    }
    if (!teleport[0] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE] == SDL_PRESSED)
    {
        timer = 0;
        if (deactivation_timer == 0) {
            deactivation_timer = SDL_GetTicks(); // Start the timer
        } else {
            int currentTime = SDL_GetTicks();
            if (currentTime - deactivation_timer > delay_duration) {
                teleport[0] = 1; // Reset the teleport flag
                p[0].general_behavior = 1.93;
                deactivation_timer = 0;
            }
        }

    }
    else if (teleport[0] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE] == SDL_PRESSED)
    {
        deactivation_timer = 0;
        if (timer == 0) {
            timer = SDL_GetTicks(); // Start the timer
        } else {
            int currentTime = SDL_GetTicks();
            if (currentTime - timer > delay_duration) {
                teleport[0] = 0; // Reset the teleport flag
                p[0].general_behavior = 1;
                timer = 0;
            }
        }
    }
    
    if (teleport[0] && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        myPlayer->position = *mousepos;
        
        timer = SDL_GetTicks(); // Reset the timer
        p[0].general_behavior = 1;
        teleport[0] = 0;
        gravity = 0;
           
        *teleportPos = (Vector2){ 0, 0 };
        myPlayer->speed = (Vector2){ 0, 0 };
    }
    
    if (currentTime - timer > time_limit)
    {
        gravity = 0.5;
    }
    else
    {
        gravity = 0;
    }
    
    (void)teleport;
    (void)teleportPos;
    (void)teleportPos;
    (void)p;
    (void)myPlayer;
}
