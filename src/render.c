#include <boids.h>

Color ColorFromHSV(double hue, double saturation, double value)
{
    double chroma = value * saturation;
    double hue1 = fmod(hue / 60.0, 6);
    double x = chroma * (1 - fabs(fmod(hue1, 2) - 1));
    double r1, g1, b1;
    if (0 <= hue1 && hue1 < 1)
    {
        r1 = chroma;
        g1 = x;
        b1 = 0;
    }
    else if (1 <= hue1 && hue1 < 2)
    {
        r1 = x;
        g1 = chroma;
        b1 = 0;
    }
    else if (2 <= hue1 && hue1 < 3)
    {
        r1 = 0;
        g1 = chroma;
        b1 = x;
    }
    else if (3 <= hue1 && hue1 < 4)
    {
        r1 = 0;
        g1 = x;
        b1 = chroma;
    }
    else if (4 <= hue1 && hue1 < 5)
    {
        r1 = x;
        g1 = 0;
        b1 = chroma;
    }
    else if (5 <= hue1 && hue1 < 6)
    {
        r1 = chroma;
        g1 = 0;
        b1 = x;
    }
    double m = value - chroma;
    double r = r1 + m;
    double g = g1 + m;
    double b = b1 + m;
    return (Color){r * 255, g * 255, b * 255, 255};
}

Color colorBoids(Boid* boid)
{
    // Define the minimum and maximum move_speeds



    // calculate current speed of boids based on prevPositions (boid->prevPositions[19] is the closest timewise to the current position)
    float move_speed = sqrt(pow(boid->prevPositions[19].x - boid->prevPositions[0].x, 2) + pow(boid->prevPositions[19].y - boid->prevPositions[0].y, 2));
    

    // Calculate the hue based on the boid's move_speed
    float hue = mapValue(move_speed , 0, 500, 0, 255);

    // Create a new color with the calculated hue
    Color color = ColorFromHSV(hue, 120, 50);   
    return color;
}


void SDL_RenderDrawCircle(SDL_Renderer *renderer, int x, int y, int radius)
{
    for (double i = 0; i < 360; i += 0.1)
    {
        SDL_RenderDrawPoint(renderer, x + radius * cos(i), y + radius * sin(i));
    }
}

void DrawBoids(Boid boids[], SDL_Renderer *renderer, player myPlayer[], int cameraX, int cameraY)
{
    Color color;
    (void) myPlayer;
    for (int i = 0; i < numBoids ; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            color = colorBoids(&boids[i]);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawLine(renderer,
                boids[i].prevPositions[j].x - cameraX, boids[i].prevPositions[j].y - cameraY,
                boids[i].prevPositions[j + 1].x - cameraX, boids[i].prevPositions[j + 1].y - cameraY);
        }

        SDL_RenderDrawPoint(renderer, boids[i].x - cameraX, boids[i].y - cameraY);
    }

    for (int j = 0; j < 19; j++)
    {
        color = colorBoids(&boids[numBoids]);
        SDL_SetRenderDrawColor(renderer, 255, 0, color.b, 255);
        SDL_RenderDrawLine(renderer,
            boids[numBoids].prevPositions[j].x - cameraX, boids[numBoids].prevPositions[j].y - cameraY,
            boids[numBoids].prevPositions[j + 1].x - cameraX, boids[numBoids].prevPositions[j + 1].y - cameraY);
    }
}


void Draw(player myPlayer[], Boid boids[], Block blocks[], int numBlocks, SDL_Renderer *renderer, HitBlock countBlocks[], int numhitbox, int cameraX, int cameraY)
{

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    DrawBoids(boids, renderer, myPlayer, cameraX, cameraY);
    (void)myPlayer;

    Color color;
    // Draw blocks with camera offset and zoom
    for (int i = 0; i < numBlocks; i++)
    {
        SDL_Rect rect = {
            (blocks[i].rect.x - cameraX),
            (blocks[i].rect.y - cameraY),
            blocks[i].rect.width,
            blocks[i].rect.height
        };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    // Draw message with camera offset and zoom
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    const char* message = "This is quite ARGH!";
    int x = (100 - cameraX) ;
    int y = (100 - cameraY) ;
    SDLTest_DrawString(renderer, x, y, message);

    // Draw hitboxes with camera offset and zoom
/*     for(int i = 0 ; i < numhitbox ; i++)
    {
        SDL_Rect rect = {
            (countBlocks[i].rect.x - cameraX) ,
            (countBlocks[i].rect.y - cameraY) ,
            countBlocks[i].rect.width ,
            countBlocks[i].rect.height 
        };

        if (countBlocks[i].hit > countBlocks[i].capacity)
        {
            countBlocks[i].hit = countBlocks[i].capacity;
        }
        color.r = mapValue(countBlocks[i].hit, 0, countBlocks[i].capacity, 0, 255);
        color.g = mapValue(countBlocks[i].hit, 0, countBlocks[i].capacity, 255, 0);
        color.b = 0;
        countBlocks[i].hit = 0;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
 */
    (void)numhitbox;
    (void)countBlocks;
    (void)color;
    SDL_RenderPresent(renderer);
}
