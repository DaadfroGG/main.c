
#include <boids.h>
// typedef t_camera

void HandleEvents(Game *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            game->quit = true;
        }
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                game->quit = true;
            }
            // handle game->p button for game->pause
            if (event.key.keysym.sym == SDLK_p)
            {
                if (game->pause == 0)
                    game->pause = 1;
                else
                    game->pause = 0;
            }
            if (game->player->in_the_end == 3 || event.key.keysym.sym == SDLK_r)
            {
                game->player->in_the_end = 0;
                game->player->score = 0;
                initPLayer(game->player, game->p, game->teleport, game);
                game->player->position = game->start_pos;
                game->camera.x = game->targetCamera.x;
                game->camera.y = game->targetCamera.y;
                game->pausecam.x = game->camera.x;
                game->pausecam.y = game->camera.y;
                InitBoids(game->boids, game->player);
                game->pause = 1;
            }
        }
        if (event.type == SDL_MOUSEWHEEL)
        {
            if (event.wheel.y > 0) // scroll up
            {
                // make the game->zoom less and less effective as it gets bigger (max game->zoom 1.0)
                if (game->zoom < 1.0)
                    game->zoom += 0.01;
                // recalculate camera position
                game->targetCamera.x = (game->player->position.x) - game->center_distance.x;
                game->targetCamera.y = (game->player->position.y) - game->center_distance.y;

                game->camera.x += (game->targetCamera.x - game->camera.x);
                game->camera.y += (game->targetCamera.y - game->camera.y);
            }
            else if (event.wheel.y < 0) // scroll down
            {
                // make the game->zoom less and less effective as it gets bigger
                if (game->zoom > 0.01)
                    game->zoom -= 0.01;

                // recalculate camera position
                game->targetCamera.x = (game->player->position.x) - game->center_distance.x;
                game->targetCamera.y = (game->player->position.y) - game->center_distance.y;
                game->camera.x += (game->targetCamera.x - game->camera.x);
                game->camera.y += (game->targetCamera.y - game->camera.y);
            }
        }
        if (event.type == SDL_MOUSEMOTION)
        {
            if (event.motion.state & SDL_BUTTON_RMASK)
            {
                // Move the map
                game->pausecam.x -= (float)event.motion.xrel / game->zoom;
                game->pausecam.y -= (float)event.motion.yrel / game->zoom;
            }
        }
    }
}

void InitCamera(Game *game)
{
    game->zoom = 0.2;
    game->bool_clement_camera = 0;
    game->camera_switch_timer = 0;
    game->center_distance = (Vector2){(screenWidth * 0.5) * game->zoom, (screenHeight * 0.5) * game->zoom};
    game->pausecam = (Vector2){(game->player->position.x) - game->center_distance.x, (game->player->position.y) - game->center_distance.y};
    game->camera = (Vector2){(game->player->position.x) - game->center_distance.x, (game->player->position.y) - game->center_distance.y};
    game->camera.x = (game->player->position.x) - game->center_distance.x;
    game->camera.y = (game->player->position.y) - game->center_distance.y;
    game->pausecam.x = (game->player->position.x) - game->center_distance.x;
    game->pausecam.y = (game->player->position.y) - game->center_distance.y;
}

void InitGame(Game *game)
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    game->window = SDL_CreateWindow("main.c", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    // Create a renderer
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_TARGETTEXTURE);
    // Initialize variables
    game->mousepos = (Vector2){-100.0f, -100.0f};
    // Array storing all positions of blocks in the level
    game->numBlocks[0] = 600;
    game->quit = false;
    game->pause = 0;
    game->player->in_the_end = 0;
    game->player->score = 0;
    game->pause = 0;

}

int main(void)
{
    Game game;
    InitGame(&game);
    InitBlocks(game.blocks, game.countBox, game.numBlocks, game.player);
    initPLayer(game.player, game.p, game.teleport, &game);
    InitBoids(game.boids, game.player);
    InitCamera(&game);
    InitBoids(game.boids, game.player);

    while (game.quit)
    {
        game.frameStart = SDL_GetTicks();

        // Calculate target camera position based on player position and game.zoom
        game.center_distance = (Vector2){(screenWidth * 0.5) / game.zoom, (screenHeight * 0.5) / game.zoom};
        game.targetCamera.x = (game.player->position.x) - game.center_distance.x;
        game.targetCamera.y = (game.player->position.y) - game.center_distance.y;
        // Apply "drag" effect to gradually move the camera towards the target position
        game.camera.x += (game.targetCamera.x - game.camera.x) * 0.05; // Adjust the drag value for desired effect (0.05)
        game.camera.y += (game.targetCamera.y - game.camera.y) * 0.05;

        // Handle events
        HandleEvents(&game);

        // Update game logic here...
        if (game.pause == 0)
        {
            game.pausecam = (Vector2){(game.player->position.x) - game.center_distance.x, (game.player->position.y) - game.center_distance.y};
            if (game.player->in_the_end == 0)
            {
                game.player->end_time = SDL_GetTicks();
                Movement(game.player);
                Control(&game.mousepos, game.teleport, game.player, game.p, game.camera.x, game.camera.y, game.boids, game.zoom, game.blocks, game.numBlocks[0]);
            }
            Update(game.boids, game.player->position, game.p[0], *game.teleport, &game);
            // Resize the map according to the game.zoom to make the player bigger

            Physics(game.player, game.blocks, game.countBox, game.numBlocks[0], game.boids);
            BoidCollision(game.boids, game.countBox, game.numBlocks[0], game.player);
            // Render graphics here...
        }
        else
        {
            game.camera.x = game.pausecam.x;
            game.camera.y = game.pausecam.y;
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT] && !game.bool_clement_camera) // unpausing the game while left is pressed, keeps the cmera as the pause one
            {
                game.bool_clement_camera = 1;
                if (game.player->in_the_end == 0)
                {
                    game.player->end_time = SDL_GetTicks();
                    Movement(game.player);
                    Control(&game.mousepos, game.teleport, game.player, game.p, game.camera.x, game.camera.y, game.boids, game.zoom, game.blocks, game.numBlocks[0]);
                }
                Update(game.boids, game.player->position, game.p[0], *game.teleport, &game);
                // Resize the map according to the game.zoom to make the player bigger
                Physics(game.player, game.blocks, game.countBox, game.numBlocks[0], game.boids);
                BoidCollision(game.boids, game.countBox, game.numBlocks[0], game.player);
            }
            else
                game.bool_clement_camera = 0;
        }
        // Clear the screen
        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);
        Draw(game.player, game.boids, game.blocks, game.numBlocks[0], game.renderer, game.countBox, game.numBlocks[0], game.camera.x, game.camera.y, game.p, game.zoom, game.pause);
        // Swap buffers
        SDL_RenderPresent(game.renderer);
        // SDL_FlushEvent(SDL_MOUSEMOTION);
        game.frameTime = SDL_GetTicks() - game.frameStart;

        if (game.frameTime < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - game.frameTime);
        }
        // start timer if player is in the end
        if (game.player->in_the_end == 1)
        {
            game.player->timer = SDL_GetTicks();
            game.player->in_the_end = 2;
        }
        // if player is in the end and 5 seconds have passed, the score is shown
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