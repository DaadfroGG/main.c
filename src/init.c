#include <boids.h>



void InitBoids(Boid boids[], player myPlayer[])
{
    srand(time(NULL));
    for (int i = 0; i < numBoids; i++) 
    {
        // boids[i].x = rand() % screenWidth;
        // boids[i].y = rand() % screenHeight;
        boids[i].x = myPlayer->position.x + rand() % 100 - 50;
        boids[i].y = myPlayer->position.y + rand() % 100 - 50;
        boids[i].angle = rand() % 360;
        boids[i].move_speed= (rand() % 1000) / 100;
        boids[i].species = i;
        boids[i].alive = 0;
        for (int j = 0; j < 20; j++) 
        {
            boids[i].prevPositions[j] = (Vector2){boids[i].x, boids[i].y};
        }
    }
}


void initPLayer(player *myPlayer, params *p, Vector2 *prevPos, Vector2 *teleportPos, int *teleport)
{
    p[0] = (params){1,100, 20, 10, 100};//last one is general speed of boids //100 is the collide value
    myPlayer->speed = (Vector2){ 0, 0 };
    myPlayer->size = 10;
    prevPos[0] = myPlayer->position;
    teleportPos[0] = myPlayer->position;
    teleport[0] = 0;
    myPlayer->grounded = 0;
    myPlayer->is_in_block = 0;
    
    myPlayer->in_the_end = 0;
    myPlayer->score = 0;
    myPlayer->clock =  SDL_GetTicks();
    myPlayer->end_time = 0;
    myPlayer->gravity = 1;
    myPlayer->shell_count = 0;
    myPlayer->camera_type = 0;

}



void InitBlocksFromFile(Block blocks[], HitBlock countBox[], int numBlocks[], player myPlayer[], const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
     
        return;
    }
    
    char line[256];
    int lineCount = 0;
    int blockIndex = 0;
    if (fgets(line, sizeof(line), file) != NULL)
    {
        lineCount++;
        float playerX, playerY;
        if (sscanf(line, "%f,%f", &playerX, &playerY) == 2)
        {
            myPlayer->position.x = playerX;
            myPlayer->position.y = playerY;
        }
        else
        {
            fprintf(stderr, "Invalid player starting position format in file %s at line %d\n", filename, lineCount);
        }
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        lineCount++;
        
        // Ignore empty lines and comments starting with '#'
        if (line[0] == '\n' || line[0] == '#')
            continue;
        
        // Parse rectangle parameters from the line
        float x, y, width, height;
        int type;
        if (sscanf(line, "%f, %f, %f, %f, %d", &x, &y, &width, &height, &type) != 5) {
            fprintf(stderr, "Invalid rectangle format in file %s at line %d\n", filename, lineCount);
            continue;
        }
        
        // Initialize the block and countBox with the parsed parameters
        blocks[blockIndex].position = (Vector2){ x, y };
        blocks[blockIndex].size = (Vector2){ width, height };
        blocks[blockIndex].rect = (Rectangle){ x, y, width, height };
        blocks[blockIndex].type = type;
        countBox[blockIndex].position = (Vector2){ x - 100, y - 100 };
        countBox[blockIndex].size = (Vector2){ width + 200, height + 200 };
        countBox[blockIndex].rect = (Rectangle){ countBox[blockIndex].position.x, countBox[blockIndex].position.y, countBox[blockIndex].size.x, countBox[blockIndex].size.y };
        countBox[blockIndex].hit = 0;
        countBox[blockIndex].type = type;
        if (type == boidbox)
        {
            countBox[blockIndex].capacity = numBoids / 2;
            countBox[blockIndex].permuability = 0;
            countBox[blockIndex].absorbtion = 0;
        }
        else if (type == end)
        {
            countBox[blockIndex].capacity = numBoids / 2;
            countBox[blockIndex].permuability = 0;
            countBox[blockIndex].absorbtion = 0;
        }
        else if (type == hitbox)
        {
            countBox[blockIndex].capacity = numBoids / 2;
            countBox[blockIndex].permuability = 1;
            countBox[blockIndex].absorbtion = 1;
        }
    /* countBox[blockIndex].capacity = numBoids / 2;
    countBox[blockIndex].permuability = 1;
     */ 
        blockIndex++;
    }
    numBlocks[0] = blockIndex;    
    fclose(file);

}


void InitBlocks(Block *blocks,HitBlock *countBox, int numBlocks[], player myPlayer[])
{
    // walls
    InitBlocksFromFile(blocks, countBox, numBlocks, myPlayer, "level.txt");
/* 
        countBox[0].position = (Vector2){ 760, 160 };
    countBox[0].size = (Vector2){ 280, 100 };
    countBox[0].rect = (Rectangle){ countBox[0].position.x, countBox[0].position.y, countBox[0].size.x, countBox[0].size.y };
    countBox[0].hit = 0;
    countBox[0].capacity = numBoids/2;
    countBox[1].position = (Vector2){ 360, 260 };
    countBox[1].size = (Vector2){ 280, 100 };
    countBox[1].rect = (Rectangle){ countBox[1].position.x, countBox[1].position.y, countBox[1].size.x, countBox[1].size.y };
    countBox[1].hit = 0;
    countBox[1].capacity = numBoids/2;
    countBox[2].position = (Vector2){ 60, 360 };
    countBox[2].size = (Vector2){ 280, 100 };
    countBox[2].rect = (Rectangle){ countBox[2].position.x, countBox[2].position.y, countBox[2].size.x, countBox[2].size.y };
    countBox[2].hit = 0;
    countBox[2].capacity = numBoids/2; */
    //can use formula instead of hardcoding
    (void)numBlocks;
}
