#include <boids.h>



/* void Movement(player* myPlayer)
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    int jump_speed = 40;
    int cap_speed = 100;
    int move_speed = cap_speed;
    myPlayer->speed.y += myPlayer->gravity; // Apply myPlayer->gravity
    if (keyboardState[SDL_SCANCODE_D])
    {
        myPlayer->speed.x += -move_speed;
    }
    else if (keyboardState[SDL_SCANCODE_A])
    {
        myPlayer->speed.x += move_speed;
    }
    //

    // Handle jumping
    if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_W] && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_S])
    {
        myPlayer->speed.y = 0;
    }
    else if (keyboardState[SDL_SCANCODE_W])
    {
        myPlayer->speed.y = -jump_speed;
    }
    else if (keyboardState[SDL_SCANCODE_S])
    {
        myPlayer->speed.y = jump_speed;
    }
    myPlayer->speed.x = sqrt(pow(myPlayer->speed.x, 2)) * (myPlayer->speed.x > 0 ? -1 : 1) * 0.9;
    
    //recude speed exponentially
    myPlayer->speed.x *= 0.70;
    //add or substract randomness to position

        //instead of that use a reverse log function
        //cap speed both positive and negative
    if (myPlayer->speed.x > cap_speed)
    {
        myPlayer->speed.x = cap_speed;
    }
    else if (myPlayer->speed.x < -cap_speed)
    {
        myPlayer->speed.x = -cap_speed;
    }

}  */


void Movement(player* myPlayer)
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    int move_speed = 5;
    int jump_speed = 20;
    myPlayer->speed.y += myPlayer->gravity; // Apply myPlayer->gravity
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
    if (myPlayer->grounded){
        if (keyboardState[SDL_SCANCODE_W])
        {
            myPlayer->speed.y = -jump_speed;
            myPlayer->grounded --;
        }
        if (keyboardState[SDL_SCANCODE_S])
        {
            myPlayer->speed.y = jump_speed;
            myPlayer->grounded --;
        }
        if (keyboardState[SDL_SCANCODE_W] && keyboardState[SDL_SCANCODE_S])
        {
            myPlayer->speed.y = 0;
            myPlayer->grounded --;
        }
        if (myPlayer->grounded < 0)
        {
            myPlayer->grounded = 0;
            myPlayer->grounded --;
        }

    }
}

int IsPointInsidePolygon(Vector2 *shell, int shell_count, int x, int y)
{
    int i, j, c = 0;
    for (i = 0, j = shell_count - 1; i < shell_count; j = i++)
    {
        if (((shell[i].y > y) != (shell[j].y > y)) &&
            (x < (shell[j].x - shell[i].x) * (y - shell[i].y) / (shell[j].y - shell[i].y) + shell[i].x))
            c = !c;
    }
    return c;
}


int IsInBlock(Block blocks[], Vector2 *point, int numBlocks)
{
    for (int i = 0; i < numBlocks; i++)
    {
        if (point->x > blocks[i].position.x && point->x < blocks[i].position.x + blocks[i].size.x && point->y > blocks[i].position.y && point->y < blocks[i].position.y + blocks[i].size.y && blocks[i].type == hitbox)
        {
            return 1;
        }
    }
    return 0;
}

void Control(Vector2* mousepos, int* teleport, Vector2* teleportPos, player* myPlayer, params* p, int cameraX, int cameraY, Boid boids[], double zoom, Block blocks[], int numBlocks)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    mousepos->x = mouseX / zoom;
    mousepos->y = mouseY / zoom;
    (void)zoom;
    // Adjust the click position based on the camera offset
    mousepos->x += cameraX;
    mousepos->y += cameraY;
    (void)boids;
    static int timer = 0;
    static int deactivation_timer = 0;
    int time_limit = 800;// Delay duration for gravity removal
    int delay_duration = 200; // Delay duration for teleportation
    int currentTime = SDL_GetTicks();
        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C]) //make camera position snap to the player
        {
            if (myPlayer->camera_type == 1)
                myPlayer->camera_type = 0;
            else
                myPlayer->camera_type = 1;
            
        }
        if (myPlayer->camera_type == 1)
        {
            cameraX = (myPlayer->position.x) - (screenWidth / 2) / zoom;
            cameraY = (myPlayer->position.y) - (screenHeight / 2) / zoom;
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
                p[0].general_behavior = 1.955; // 1.93 is the teleport behavior// 2.3 IS QUANTUM;
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
    
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {   
    if (IsPointInsidePolygon(myPlayer->shell, myPlayer->shell_count, mousepos->x, mousepos->y) && !IsInBlock(blocks, mousepos, numBlocks))
    {
        (void)blocks;
        (void)numBlocks;
             myPlayer->position = *mousepos;
            timer = SDL_GetTicks(); // Reset the timer
            p[0].general_behavior = 1;
            teleport[0] = 0;
            myPlayer->gravity = 0;
            // reset the speed
            // myPlayer->speed.x = 0;
            myPlayer->speed.y = 0;
            // ;
            //calculate the speed based on the distance between the teleport position and the player
       }
    }
    
    if (currentTime - timer > time_limit)
    {
        myPlayer->gravity = 0.8;
    }
    else
    {
        myPlayer->gravity = 0;
        //keep some momentum to the direction of teleportation (right or left)
        //calculate the momemtum based on how many frames have passed since the teleportation



    }
    
    (void)teleport;
    (void)teleportPos;
    (void)teleportPos;
    (void)p;
    (void)myPlayer;
}
