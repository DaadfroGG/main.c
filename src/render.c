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
    SDL_Vertex vertex_1 = {{position[0].x, position[0].y}, {color.r, color.g, color.b, color.a}, {1, 1}};
    SDL_Vertex vertex_2 = {{position[1].x, position[1].y}, {color.r, color.g, color.b, color.a}, {1, 1}};
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

void DrawBoxes(Block blocks[], int numBlocks, SDL_Renderer *renderer, int cameraX, int cameraY, double zoom)
{
    for (int i = 0; i < numBlocks; i++)
    {   

            Color color_line;
            Color color;
            double offsetX = 70;
            double offsetY = 0;
            if (blocks[i].type == hitbox)
                color_line = (Color){255, 0, 0, 255};
            else if (blocks[i].type == boidbox)
                color_line = (Color){0, 0, 255, 255};
            else if (blocks[i].type == end)
                color_line = (Color){0, 255, 0, 255};
            color = (Color){0, 0, 0, 255};
            // SDL_Vertex face2[2][3];// no need for back face
            // NewBlockVertex((SDL_Rect){
            //     (blocks[i].rect.x - cameraX) * zoom * 0.9 + 150,
            //     (blocks[i].rect.y - cameraY) * zoom * 0.9 + 70,
            //     blocks[i].rect.width * zoom * 0.9,
            //     blocks[i].rect.height * zoom * 0.9
            // }, face2, color);
            // DrawTriangle(renderer, face2[0], color);
            //draw outline of the back face
            SDL_SetRenderDrawColor(renderer, color_line.r, color_line.g, color_line.b, color_line.a);
            /* SDL_RenderDrawRect(renderer, &(SDL_Rect){
                (blocks[i].rect.x - cameraX) * zoom * 0.9 + 150,
                (blocks[i].rect.y - cameraY) * zoom * 0.9 + 70,
                blocks[i].rect.width * zoom * 0.9,
                blocks[i].rect.height * zoom * 0.9
            }); 
            REMMERBER TO ADD THICKNESS TO THE OUTLINE
            */
           int line_thickness = 1;
           SDL_Rect front = {
               (blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX,
               (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY,
               blocks[i].rect.width * zoom * 1.1,
               blocks[i].rect.height * zoom * 1.1
           };
              SDL_Rect back = {
                (blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX,
                (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY,
                blocks[i].rect.width * zoom * 0.9,
                blocks[i].rect.height * zoom * 0.9
              };
            SDL_Rect backOutline = {
                back.x - line_thickness,
                back.y - line_thickness,
                back.w + (line_thickness * 4),
                back.h + (line_thickness * 4)
            };
            SDL_Rect frontOutline = {
                front.x - line_thickness,
                front.y - line_thickness,
                front.w + (line_thickness * 4),
                front.h + (line_thickness * 4)
            };
           SDL_RenderDrawRect(renderer, &backOutline);
            //draw lines connecting the front face to the back face
            SDL_RenderDrawLine(renderer,
                backOutline.x, backOutline.y,
                frontOutline.x, frontOutline.y);
            SDL_RenderDrawLine(renderer,
                backOutline.x + backOutline.w, backOutline.y,
                frontOutline.x + frontOutline.w, frontOutline.y);
            SDL_RenderDrawLine(renderer,
                backOutline.x, backOutline.y + backOutline.h,
                frontOutline.x, frontOutline.y + frontOutline.h);
            SDL_RenderDrawLine(renderer,
                backOutline.x + backOutline.w, backOutline.y + backOutline.h,
                frontOutline.x + frontOutline.w, frontOutline.y + frontOutline.h);
            //draw the front face
            SDL_Vertex face2[2][3];//first side face
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9}
            }, face2[0], color);
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9},
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1}
            }, face2[1], color);
            DrawTriangle(renderer, face2[0], color);
            DrawTriangle(renderer, face2[1], color);
            SDL_Vertex face3[2][3];//second side face
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX + blocks[i].rect.width * zoom * 1.1, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9}
            }, face3[0], color);
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX + blocks[i].rect.width * zoom * 1.1, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9},
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX + blocks[i].rect.width * zoom * 1.1, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1}
            }, face3[1], color);
            DrawTriangle(renderer, face3[0], color);
            DrawTriangle(renderer, face3[1], color);
            SDL_Vertex face4[2][3];//third side face
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9}
            }, face4[0], color);
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9},
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX + blocks[i].rect.width * zoom * 1.1, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1}
            }, face4[1], color);
            DrawTriangle(renderer, face4[0], color);
            DrawTriangle(renderer, face4[1], color);
            SDL_Vertex face5[2][3];//top face
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX + blocks[i].rect.width * zoom * 1.1, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX + blocks[i].rect.width * zoom * 1.1, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1}
            }, face5[0], color);
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX + blocks[i].rect.width * zoom * 1.1, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1},
                {(blocks[i].rect.x - cameraX) * zoom * 1.1 - offsetX, (blocks[i].rect.y - cameraY) * zoom * 1.1 - offsetY + blocks[i].rect.height * zoom * 1.1}
            }, face5[1], color);
            DrawTriangle(renderer, face5[0], color);
            DrawTriangle(renderer, face5[1], color);
            SDL_Vertex face6[2][3];//bottom face
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9}
            }, face6[0], color);
            NewTriangleVertex((Vector2[]){
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX + blocks[i].rect.width * zoom * 0.9, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9},
                {(blocks[i].rect.x - cameraX) * zoom * 0.9 + offsetX, (blocks[i].rect.y - cameraY) * zoom * 0.9 + offsetY + blocks[i].rect.height * zoom * 0.9}
            }, face6[1], color);
            DrawTriangle(renderer, face6[0], color);
            DrawTriangle(renderer, face6[1], color);

            SDL_Vertex face1[2][3];//two triangles per rectangle, only three faces to show, depending on camera angle
            NewBlockVertex(front, face1, color);
            SDL_SetRenderDrawColor(renderer, color_line.r, color_line.g, color_line.b, color_line.a);
            DrawTriangle(renderer, face1[0], color_line);
            DrawTriangle(renderer, face1[1], color_line);
            //render outline of front face
            SDL_RenderDrawRect(renderer, &frontOutline);


    }
}
/* 

void DrawBoxes(Block blocks[], int numBlocks, SDL_Renderer *renderer, int cameraX, int cameraY, double zoom)
{
    for (int i = 0; i < numBlocks; i++)
    {
        // SDL_Rect frontBlock = {
        //     (blocks[i].rect.x - cameraX) * zoom,
        //     (blocks[i].rect.y - cameraY) * zoom,
        //     blocks[i].rect.width * zoom,
        //     blocks[i].rect.height * zoom
        // };

        // Parallax effect
        SDL_Rect backBlock = {
            (blocks[i].rect.x - cameraX) * zoom * 0.9 + 150,
            (blocks[i].rect.y - cameraY) * zoom * 0.9 + 70,
            blocks[i].rect.width * zoom * 0.9,
            blocks[i].rect.height * zoom * 0.9
        };

        // Block in front of the front block
        SDL_Rect frontestBlock = {
            (blocks[i].rect.x - cameraX) * zoom * 1.1 - 150,
            (blocks[i].rect.y - cameraY) * zoom * 1.1 - 70,
            blocks[i].rect.width * zoom * 1.1,
            blocks[i].rect.height * zoom * 1.1

        };
        //faces of the cube (connected lines between frontest and back

        Color color;
        if (blocks[i].type == hitbox)
            color = (Color){255, 0, 0, 255};
        else if (blocks[i].type == boidbox)
            color = (Color){0, 0, 255, 255};
        else if (blocks[i].type == end)
            color = (Color){0, 255, 0, 255};

        // Draw the back rect
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawRect(renderer, &backBlock);

        // Draw the lines connecting the back and frontest rects
        SDL_RenderDrawLine(renderer, backBlock.x, backBlock.y, frontestBlock.x, frontestBlock.y);
        SDL_RenderDrawLine(renderer, backBlock.x + backBlock.w, backBlock.y, frontestBlock.x + frontestBlock.w, frontestBlock.y);
        SDL_RenderDrawLine(renderer, backBlock.x, backBlock.y + backBlock.h, frontestBlock.x, frontestBlock.y + frontestBlock.h);
        SDL_RenderDrawLine(renderer, backBlock.x + backBlock.w, backBlock.y + backBlock.h, frontestBlock.x + frontestBlock.w, frontestBlock.y + frontestBlock.h);

        // Create the polygons made from the lines
        // SDL_Vertex vertices[] = {
        //     {{backBlock.x, backBlock.y}, {color.r, color.g, color.b, 255}, {1, 1}},
        //     {{backBlock.x + backBlock.w, backBlock.y}, {color.r, color.g, color.b, 255}, {1, 1}},
        //     {{frontestBlock.x + frontestBlock.w, frontestBlock.y}, {color.r, color.g, color.b, 255}, {1, 1}},
        //     {{frontestBlock.x, frontestBlock.y}, {color.r, color.g, color.b, 255}, {1, 1}}
        // };
        //draw the polygons
        // DrawPoly(renderer, vertices, 4);


        // Draw the front rect
        // SDL_RenderDrawRect(renderer, &frontBlock);

        // Draw the frontest rect
        SDL_RenderDrawRect(renderer, &frontestBlock);
    
    }
}
 */


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
    DrawBoxes(blocks, numBlocks, renderer, cameraX, cameraY, zoom);
    /*
    for (int i = 0; i < numBlocks; i++)
    {
        SDL_Rect rect = {
            (blocks[i].rect.x - cameraX)* zoom,
            (blocks[i].rect.y - cameraY)* zoom,
            blocks[i].rect.width * zoom,
            blocks[i].rect.height * zoom
        };
        Color color;        
        color = (Color){0, 0, 0, 255};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderFillRect(renderer, &rect);

        if ( blocks[i].type == hitbox)
            color = (Color){255, 0, 0, 255};
        else if ( blocks[i].type == boidbox)
            color = (Color){0, 0, 255, 255};
        else if ( blocks[i].type == end)
            color = (Color){0, 255, 0, 255};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawRect(renderer, &rect);
        //fill rectangle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        char message [100];
        int x = (blocks[i].rect.x - cameraX)* zoom;
        int y = (blocks[i].rect.y - cameraY)* zoom;
        sprintf(message, "%d", countBlocks[i].hit);
        SDLTest_DrawString(renderer, x, y, message);
    }
    */
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
    // Draw hitboxes with camera offset and zoom
    // DrawBoxes(countBlocks, numhitbox, renderer, cameraX, cameraY, zoom);
    DrawBoids(boids, renderer, myPlayer, cameraX, cameraY, zoom);
    DrawStats(myPlayer, renderer);
    DrawHull(myPlayer, renderer, cameraX, cameraY, zoom);
    (void)numhitbox;
    (void)countBlocks;

    SDL_RenderPresent(renderer);
}
