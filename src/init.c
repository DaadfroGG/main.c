#include <boids.h>



void InitBoids(Boid boids[])
{
    srand(time(NULL));
    for (int i = 0; i < numBoids; i++) 
    {
        boids[i].x = rand() % screenWidth;
        boids[i].y = rand() % screenHeight;
        boids[i].angle = rand() % 360;
        boids[i].move_speed= (rand() % 1000) / 100;
        boids[i].species = i;
        boids[i].alive = 1;
        for (int j = 0; j < 20; j++) 
        {
            boids[i].prevPositions[j] = (Vector2){boids[i].x, boids[i].y};
        }
    }
}


void initPLayer(player *myPlayer, params *p, Vector2 *prevPos, Vector2 *teleportPos, int *teleport)
{
    p[0] = (params){1,100, 20, 10, 40};
    myPlayer->position = (Vector2){ screenWidth / 2, screenHeight / 2 };
    myPlayer->speed = (Vector2){ 0, 0 };
    myPlayer->size = 10;
    prevPos[0] = myPlayer->position;
    teleportPos[0] = myPlayer->position;
    teleport[0] = 0;
    myPlayer->grounded = 0;
}
void InitBlocks(Block blocks[],HitBlock countBox[], int numBlocks)
{
    // walls
    blocks[0].position = (Vector2){ 0, 0 };;
    blocks[0].size = (Vector2){ 700, 20 };
    blocks[0].rect = (Rectangle){ blocks[0].position.x, blocks[0].position.y, blocks[0].size.x, blocks[0].size.y };
    blocks[1].position = (Vector2){ 0, 0 };;
    blocks[1].size = (Vector2){ 20, screenHeight };
    blocks[1].rect = (Rectangle){ blocks[1].position.x, blocks[1].position.y, blocks[1].size.x, blocks[1].size.y };
    blocks[2].position = (Vector2){ screenWidth - 20, 0 };
    blocks[2].size = (Vector2){ 20, screenHeight };
    blocks[2].rect = (Rectangle){ blocks[2].position.x, blocks[2].position.y, blocks[2].size.x, blocks[2].size.y };
    blocks[3].position = (Vector2){ 0, screenHeight - 20 };
    blocks[3].size = (Vector2){ screenWidth, 20 };
    blocks[3].rect = (Rectangle){ blocks[3].position.x, blocks[3].position.y, blocks[3].size.x, blocks[3].size.y };
    //plateforms
    blocks[4].position = (Vector2){ 800, 200 };
    blocks[4].size = (Vector2){ 200, 20 };
    blocks[4].rect = (Rectangle){ blocks[4].position.x, blocks[4].position.y, blocks[4].size.x, blocks[4].size.y };
    blocks[5].position = (Vector2){ 400, 300 };
    blocks[5].size = (Vector2){ 200, 20 };
    blocks[5].rect = (Rectangle){ blocks[5].position.x, blocks[5].position.y, blocks[5].size.x, blocks[5].size.y };
    blocks[6].position = (Vector2){ 100, 400 };
    blocks[6].size = (Vector2){ 200, 20 };
    blocks[6].rect = (Rectangle){ blocks[6].position.x, blocks[6].position.y, blocks[6].size.x, blocks[6].size.y };
    blocks[7].position = (Vector2){ 0, screenHeight - 150 };
    blocks[7].size = (Vector2){ screenWidth - 100, 20 };
    blocks[7].rect = (Rectangle){ blocks[7].position.x, blocks[7].position.y, blocks[7].size.x, blocks[7].size.y };
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
    for (int i = 0; i < numBlocks; i++) 
    {
        countBox[i].position = (Vector2){ blocks[i].position.x - 40, blocks[i].position.y - 40 };
        countBox[i].size = (Vector2){ blocks[i].size.x + 80, blocks[i].size.y + 80 };
        countBox[i].rect = (Rectangle){ countBox[i].position.x, countBox[i].position.y, countBox[i].size.x, countBox[i].size.y };
        countBox[i].hit = 0;
        countBox[i].capacity = numBoids/2;
        countBox[i].permuability = 1;
        countBox[i].absorbtion = 1;
    }
    (void)numBlocks;
}
