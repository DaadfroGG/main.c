

#include <boids.h>


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

void BoidCollision(Boid boids[], HitBlock countBox[], int numBlocks, player myPlayer[])
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