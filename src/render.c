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

void DrawBoids(Boid boids[], SDL_Renderer *renderer, player myPlayer[], int cameraX, int cameraY, double zoom)
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
                (boids[i].prevPositions[j].x - cameraX)* zoom, (boids[i].prevPositions[j].y - cameraY)* zoom,
                (boids[i].prevPositions[j + 1].x - cameraX)* zoom, (boids[i].prevPositions[j + 1].y - cameraY)* zoom);
        }

        SDL_RenderDrawPoint(renderer, (boids[i].x - cameraX)* zoom, (boids[i].y - cameraY)* zoom);
    }

    for (int j = 0; j < 19; j++)
    {
        color = colorBoids(&boids[numBoids]);
        SDL_SetRenderDrawColor(renderer, 255, 0, color.b, 255);
        SDL_RenderDrawLine(renderer,
            (boids[numBoids].prevPositions[j].x - cameraX)* zoom, (boids[numBoids].prevPositions[j].y - cameraY)* zoom,
            (boids[numBoids].prevPositions[j + 1].x - cameraX)* zoom, (boids[numBoids].prevPositions[j + 1].y - cameraY)* zoom);
    }
}


void DrawHull(player *myPlayer, SDL_Renderer *renderer, int cameraX, int cameraY, double zoom)
{
    for (int i = 0; i < myPlayer->shell_count; i++)
{
    SDL_SetRenderDrawColor(renderer, 120, 120, 250, 255);
    SDL_RenderDrawLine(renderer, (myPlayer->shell[i].x - cameraX) * zoom, (myPlayer->shell[i].y - cameraY) * zoom, (myPlayer->shell[i + 1].x - cameraX) * zoom, (myPlayer->shell[i + 1].y - cameraY) * zoom);
}

// Draw the last line segment connecting the last point with the first point
SDL_SetRenderDrawColor(renderer, 120, 120, 250, 255);
SDL_RenderDrawLine(renderer, (myPlayer->shell[myPlayer->shell_count].x - cameraX) * zoom, (myPlayer->shell[myPlayer->shell_count].y - cameraY) * zoom, (myPlayer->shell[0].x - cameraX) * zoom, (myPlayer->shell[0].y - cameraY) * zoom);

}

#include <SDL2/SDL.h>
#include <math.h>

void DrawStats(player myPlayer[], SDL_Renderer *renderer)
{
    int jump_tick_max = 20;
    int current_jump_tick = myPlayer->grounded;
    float jump_percentage = (float)current_jump_tick / (float)jump_tick_max;

    // Assuming the camembert center and radius
    int camembert_center_x = 100;
    int camembert_center_y = 100;
    int camembert_radius = 50;

    // Background color

    // Draw camembert outline
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int angle = 0; angle <= 360; angle += 1)
    {
        float radians = (angle - 90) * M_PI / 180.0;
        int x = camembert_center_x + (int)(camembert_radius * cos(radians));
        int y = camembert_center_y + (int)(camembert_radius * sin(radians));
        SDL_RenderDrawLine(renderer, camembert_center_x, camembert_center_y, x, y);
    }

    // Fill the camembert based on the jump percentage
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the jump
    for (int angle = 0; angle <= 360 * jump_percentage; angle += 1)
    {
        float radians = (angle - 90) * M_PI / 180.0;
        int x = camembert_center_x + (int)(camembert_radius * cos(radians));
        int y = camembert_center_y + (int)(camembert_radius * sin(radians));
        SDL_RenderDrawLine(renderer, camembert_center_x, camembert_center_y, x, y);
    }

}

void NewTriangleVertex(Vector2 position[3], SDL_Vertex vertices[3], Color color)
{
    // SDL_Vertex vertex_1 = {{position[0].x, position[0].y}, {color.r, color.g, color.b, color.a}, {1, 1}};
    // SDL_Vertex vertex_2 = {{position[1].x, position[1].y}, {color.r, color.g, color.b, color.a}, {1, 1}};
    // SDL_Vertex vertex_3 = {{position[2].x, position[2].y}, {color.r, color.g, color.b, color.a}, {1, 1}};
    SDL_Vertex vertex_1 = {{position[0].x, position[0].y}, {color.b, color.r, color.g, color.a}, {1, 1}};
    SDL_Vertex vertex_2 = {{position[1].x, position[1].y}, {color.r, color.b, color.r, color.a}, {1, 1}};
    SDL_Vertex vertex_3 = {{position[2].x, position[2].y}, {color.r, color.g, color.b, color.a}, {1, 1}};
    vertices[0] = vertex_1;
    vertices[1] = vertex_2;
    vertices[2] = vertex_3;
}

void NewBlockVertex(SDL_Rect rect, SDL_Vertex vertices[2][3], Color color)
{
    Vector2 position[3];
    position[0] = (Vector2){rect.x, rect.y};
    position[1] = (Vector2){rect.x + rect.w, rect.y};
    position[2] = (Vector2){rect.x, rect.y + rect.h};
    NewTriangleVertex(position, vertices[0], color);
    position[0] = (Vector2){rect.x + rect.w, rect.y};
    position[1] = (Vector2){rect.x + rect.w, rect.y + rect.h};
    position[2] = (Vector2){rect.x, rect.y + rect.h};
    NewTriangleVertex(position, vertices[1], color);
}

void DrawTriangle(SDL_Renderer *renderer, SDL_Vertex vertices[], Color color)
{
    // Set renderer color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // Render red triangle
    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

void DrawPoly(SDL_Renderer *renderer, SDL_Vertex vertices[], int numVertices)
{
    // Set renderer color
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    // Render red triangle
    SDL_RenderGeometry(renderer, NULL, vertices, numVertices, NULL, 0);
}

void InitFaces(SDL_Vertex faces[5][2][3], SDL_Rect front_rect, SDL_Rect back_rect, Color color)
{
    Color polycolor = color;
    //draw the left face
    SDL_Vertex left[2][3];//left side face, connect the two lefts of the front_rect and back faces
    polycolor.r = color.r * 0.8;
    polycolor.g = color.g * 0.8;
    polycolor.b = color.b * 0.8;
    NewTriangleVertex((Vector2[]){{front_rect.x, front_rect.y}, {back_rect.x, back_rect.y}, {back_rect.x, back_rect.y + back_rect.h}}, left[0], polycolor);
    NewTriangleVertex((Vector2[]){{front_rect.x, front_rect.y}, {front_rect.x, front_rect.y + front_rect.h}, {back_rect.x, back_rect.y + back_rect.h}}, left[1], polycolor);
    faces[0][0][0] = left[0][0];
    faces[0][0][1] = left[0][1];
    faces[0][0][2] = left[0][2];
    faces[0][1][0] = left[1][0];
    faces[0][1][1] = left[1][1];
    faces[0][1][2] = left[1][2];
    //draw the right face
    SDL_Vertex right[2][3];//right side face, connect the two rights of the front_rect and back faces
    polycolor.r = color.r * 0.6;
    polycolor.g = color.g * 0.6;
    polycolor.b = color.b * 0.6;
    NewTriangleVertex((Vector2[]){{front_rect.x + front_rect.w, front_rect.y}, {back_rect.x + back_rect.w, back_rect.y}, {back_rect.x + back_rect.w, back_rect.y + back_rect.h}}, right[0], polycolor);
    NewTriangleVertex((Vector2[]){{front_rect.x + front_rect.w, front_rect.y}, {front_rect.x + front_rect.w, front_rect.y + front_rect.h}, {back_rect.x + back_rect.w, back_rect.y + back_rect.h}}, right[1], polycolor);
    faces[1][0][0] = right[0][0];
    faces[1][0][1] = right[0][1];
    faces[1][0][2] = right[0][2];
    faces[1][1][0] = right[1][0];
    faces[1][1][1] = right[1][1];
    faces[1][1][2] = right[1][2];
    //draw the up face
    SDL_Vertex up[2][3];//up side face, connect the two tops of the front_rect and back faces
    polycolor.r = color.r * 0.5;
    polycolor.g = color.g * 0.5;
    polycolor.b = color.b * 0.5;
    NewTriangleVertex((Vector2[]){{front_rect.x, front_rect.y}, {front_rect.x + front_rect.w, front_rect.y}, {back_rect.x, back_rect.y}}, up[0], polycolor);
    NewTriangleVertex((Vector2[]){{front_rect.x + front_rect.w, front_rect.y}, {back_rect.x + back_rect.w, back_rect.y}, {back_rect.x, back_rect.y}}, up[1], polycolor);
    faces[2][0][0] = up[0][0];
    faces[2][0][1] = up[0][1];
    faces[2][0][2] = up[0][2];
    faces[2][1][0] = up[1][0];
    faces[2][1][1] = up[1][1];
    faces[2][1][2] = up[1][2];
    //draw the down face
    polycolor.r = color.r * 0.4;
    polycolor.g = color.g * 0.4;
    polycolor.b = color.b * 0.4;
    SDL_Vertex down[2][3];//down side face, connect the two bottoms of the front_rect and back faces
    NewTriangleVertex((Vector2[]){{front_rect.x, front_rect.y + front_rect.h}, {front_rect.x + front_rect.w, front_rect.y + front_rect.h}, {back_rect.x, back_rect.y + back_rect.h}}, down[0], polycolor);
    NewTriangleVertex((Vector2[]){{front_rect.x + front_rect.w, front_rect.y + front_rect.h}, {back_rect.x + back_rect.w, back_rect.y + back_rect.h}, {back_rect.x, back_rect.y + back_rect.h}}, down[1], polycolor);
    faces[3][0][0] = down[0][0];
    faces[3][0][1] = down[0][1];
    faces[3][0][2] = down[0][2];
    faces[3][1][0] = down[1][0];
    faces[3][1][1] = down[1][1];
    faces[3][1][2] = down[1][2];

    //draw the front face
    //set color to white
    SDL_Vertex front[2][3];//front side face
    NewTriangleVertex((Vector2[]){{front_rect.x, front_rect.y}, {front_rect.x + front_rect.w, front_rect.y}, {front_rect.x, front_rect.y + front_rect.h}}, front[0], color);
    NewTriangleVertex((Vector2[]){{front_rect.x + front_rect.w, front_rect.y}, {front_rect.x + front_rect.w, front_rect.y + front_rect.h}, {front_rect.x, front_rect.y + front_rect.h}}, front[1], color);
    faces[4][0][0] = front[0][0];
    faces[4][0][1] = front[0][1];
    faces[4][0][2] = front[0][2];
    faces[4][1][0] = front[1][0];
    faces[4][1][1] = front[1][1];
    faces[4][1][2] = front[1][2];

}

    double offsetX = 10;
    double offsetY = -10;
    int line_thickness = 1;
    double zoom_offset = 0.5 ;

void DrawFront(Block blocks[], int numBlocks, SDL_Renderer *renderer, int cameraX, int cameraY, double zoom)
{
    // SortBoxes(blocks, numBlocks, cameraX, cameraY, zoom);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    offsetY = -10 * zoom;
    offsetX = 400* zoom;
    
    zoom_offset = 0.5 *zoom;
    for (int i = 0; i < numBlocks; i++)
    {   

            Color color_line;
            Color color;
            color = (Color){0, 0, 0, 255};
            color_line = (Color){0, 0, 0, 255};
                      SDL_Rect front_rect = {
               (blocks[i].rect.x - cameraX) * zoom * (1 + zoom_offset) - offsetX,
                (blocks[i].rect.y - cameraY) * zoom * (1 + zoom_offset) - offsetY,
                blocks[i].rect.width * zoom * (1 + zoom_offset),
                blocks[i].rect.height * zoom * (1 + zoom_offset)
           };
            //   SDL_Rect back_rect = {
            //     (blocks[i].rect.x - cameraX) * zoom * (1 - zoom_offset) + offsetX,
            //     (blocks[i].rect.y - cameraY) * zoom * (1 - zoom_offset) + offsetY,
            //     blocks[i].rect.width * zoom * (1 - zoom_offset),
            //     blocks[i].rect.height * zoom * (1 - zoom_offset)
            //   };

              SDL_Rect back_rect = {
                (blocks[i].rect.x - cameraX) * zoom,
                (blocks[i].rect.y - cameraY) * zoom,
                blocks[i].rect.width * zoom,
                blocks[i].rect.height * zoom
              };
        //    SDL_RenderDrawRect(renderer, &backOutline);
            SDL_Vertex faces[5][2][3]; // 5 faces * 2 triangles per face
            /*     hitbox,
                    boidbox,
                    end */
            if (blocks[i].type == hitbox)
                color_line = (Color){255, 0, 0, 255};
            else if (blocks[i].type == boidbox)
                color_line = (Color){0, 0, 255, 150};
            else if (blocks[i].type == end)
                color_line = (Color){0, 255, 0, 150};
            SDL_SetRenderDrawColor(renderer, color_line.r, color_line.g, color_line.b, color_line.a);
            InitFaces(faces, back_rect, front_rect, color_line);

   
            //draw the left face only if the face is on the left side of the block
            if (front_rect.x - back_rect.x > 0)
            {
                //draw the left face
                DrawTriangle(renderer, faces[0][0], color);
                DrawTriangle(renderer, faces[0][1], color);
            }
            if ((front_rect.x + front_rect.w) - (back_rect.x + back_rect.w) < 0)
            {
                //draw the right face
                DrawTriangle(renderer, faces[1][0], color);
                DrawTriangle(renderer, faces[1][1], color);
            }
            if (front_rect.y - back_rect.y > 0)
            {
                //draw the up face
                DrawTriangle(renderer, faces[2][0], color);
                DrawTriangle(renderer, faces[2][1], color);
            }
            if ((front_rect.y + front_rect.h) - (back_rect.y + back_rect.h) < 0)
            {
                //draw the down face
                DrawTriangle(renderer, faces[3][0], color);
                DrawTriangle(renderer, faces[3][1], color);
            }
    }

}

void DrawBack(Block blocks[], int numBlocks, SDL_Renderer *renderer, int cameraX, int cameraY, double zoom)
{
    // SortBoxes(blocks, numBlocks, cameraX, cameraY, zoom);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    offsetY = -10 * zoom;
    offsetX = 400* zoom;
    
    zoom_offset = 0.5 *zoom;
    for (int i = 0; i < numBlocks; i++)
    {   

            Color color_line;
            Color color;
            color = (Color){0, 0, 0, 255};
            color_line = (Color){0, 0, 0, 255};
           SDL_Rect front_rect = {
               (blocks[i].rect.x - cameraX) * zoom,
                (blocks[i].rect.y - cameraY) * zoom,
                blocks[i].rect.width * zoom,
                blocks[i].rect.height * zoom
           };
        //               SDL_Rect front_rect = {
        //        (blocks[i].rect.x - cameraX) * zoom * (1 + zoom_offset) - offsetX,
        //         (blocks[i].rect.y - cameraY) * zoom * (1 + zoom_offset) - offsetY,
        //         blocks[i].rect.width * zoom * (1 + zoom_offset),
        //         blocks[i].rect.height * zoom * (1 + zoom_offset)
        //    };
              SDL_Rect back_rect = {
                (blocks[i].rect.x - cameraX) * zoom * (1 - zoom_offset) + offsetX,
                (blocks[i].rect.y - cameraY) * zoom * (1 - zoom_offset) + offsetY,
                blocks[i].rect.width * zoom * (1 - zoom_offset),
                blocks[i].rect.height * zoom * (1 - zoom_offset)
              };

        //    SDL_RenderDrawRect(renderer, &backOutline);
            SDL_Vertex faces[5][2][3]; // 5 faces * 2 triangles per face
            /*     hitbox,
                    boidbox,
                    end */
            if (blocks[i].type == hitbox)
                color_line = (Color){255, 0, 0, 255};
            else if (blocks[i].type == boidbox)
                color_line = (Color){0, 0, 255, 150};
            else if (blocks[i].type == end)
                color_line = (Color){0, 255, 0, 150};
            SDL_SetRenderDrawColor(renderer, color_line.r, color_line.g, color_line.b, color_line.a);
            InitFaces(faces, back_rect, front_rect, color_line);

   
            //draw the left face only if the face is on the left side of the block
            if (front_rect.x - back_rect.x > 0)
            {
                //draw the left face
                DrawTriangle(renderer, faces[0][0], color);
                DrawTriangle(renderer, faces[0][1], color);
            }
            if ((front_rect.x + front_rect.w) - (back_rect.x + back_rect.w) < 0)
            {
                //draw the right face
                DrawTriangle(renderer, faces[1][0], color);
                DrawTriangle(renderer, faces[1][1], color);
            }
            if (front_rect.y - back_rect.y > 0)
            {
                //draw the up face
                DrawTriangle(renderer, faces[2][0], color);
                DrawTriangle(renderer, faces[2][1], color);
            }
            if ((front_rect.y + front_rect.h) - (back_rect.y + back_rect.h) < 0)
            {
                //draw the down face
                DrawTriangle(renderer, faces[3][0], color);
                DrawTriangle(renderer, faces[3][1], color);
            }
    }
    

}


void Draw(player myPlayer[], Boid boids[], Block blocks[], int numBlocks, SDL_Renderer *renderer, HitBlock countBlocks[], int numhitbox, int cameraX, int cameraY, params p[], double zoom, int pause)
{
    //draw infinite grid from where the camera is
    (void) p;
 // Draw the infinite grid
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // Set grid color
    double gridSize = 1000;
    int startGridX = cameraX / gridSize;
    int startGridY = cameraY / gridSize;
    int endGridX = startGridX + screenWidth;
    int endGridY = startGridY + screenHeight;

    for (int i = startGridX; i <= endGridX; i++)
    {
        SDL_RenderDrawLine(renderer, (i * gridSize - cameraX) * zoom, -cameraY * zoom, (i * gridSize - cameraX) * zoom, (endGridY * gridSize - cameraY) * zoom);
    }

    for (int i = startGridY; i <= endGridY; i++)
    {
        SDL_RenderDrawLine(renderer, -cameraX * zoom, (i * gridSize - cameraY) * zoom, (endGridX * gridSize - cameraX) * zoom, (i * gridSize - cameraY) * zoom);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Draw blocks with camera offset and zoom
    //cameraX, cameraY, zoom, blocks, numBlocks
    DrawBack(blocks, numBlocks, renderer, cameraX, cameraY, zoom);

    // Draw end message if player is at the end
    float seconds = myPlayer[0].end_time - myPlayer[0].clock;
    seconds /= 1000;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    char message[100];
    sprintf(message, "Your score is: %d", myPlayer->score);
    SDLTest_DrawString(renderer, 0, 0, message);
    sprintf(message, "Your time is: %0.3fs", seconds);
    SDLTest_DrawString(renderer, 0, 20, message);
    sprintf(message, "Press R to restart");
    SDLTest_DrawString(renderer, 0, 40, message);

    if (pause)
    {
                   //draw pause message
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        char message[100];
        sprintf(message, "Pause");
        SDLTest_DrawString(renderer, 400, 100, message);
    }

    DrawBoids(boids, renderer, myPlayer, cameraX, cameraY, zoom);
    DrawFront(blocks, numBlocks, renderer, cameraX, cameraY, zoom);
    for (int i = 0; i < numBlocks; i++)
    {
            SDL_SetRenderDrawColor(renderer, 0, 0, 60, 10);
            SDL_Vertex vertices[2][3];
            Color color;
            color = (Color){0, 0, 0, 255};
            if (blocks[i].type == hitbox)
                color = (Color){255, 0, 0, 200};
            else if (blocks[i].type == boidbox)
                color = (Color){0, 0, 255, 120};
            else if (blocks[i].type == end)
                color = (Color){0, 255, 0, 120};
            NewBlockVertex((SDL_Rect){
                (blocks[i].rect.x - cameraX) * zoom * (1 + zoom_offset) - offsetX,
                (blocks[i].rect.y - cameraY) * zoom * (1 + zoom_offset) - offsetY,
                blocks[i].rect.width * zoom * (1 + zoom_offset),
                blocks[i].rect.height * zoom * (1 + zoom_offset)
            }, vertices, color);
            DrawPoly(renderer, vertices[0], 3);
            DrawPoly(renderer, vertices[1], 3);
             SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            char message [100];
            int x = (blocks[i].rect.x - cameraX)* zoom * (1 + zoom_offset) - offsetX;
            int y = (blocks[i].rect.y - cameraY)* zoom * (1 + zoom_offset) - offsetY;
            sprintf(message, "%d", countBlocks[i].hit);
            SDLTest_DrawString(renderer, x, y, message);

    }
    DrawStats(myPlayer, renderer);
    DrawHull(myPlayer, renderer, cameraX, cameraY, zoom);
    (void)numhitbox;
    (void)countBlocks;

    SDL_RenderPresent(renderer);
}
