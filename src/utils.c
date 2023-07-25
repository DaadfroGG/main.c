#include <boids.h>

// Path: src/utils.c

void moveBoids(Boid *boids)
{
    for (int i = 0; i < numBoids; i++) 
    {
        if (!boids[i].alive)
        {
            boids[i].angle = 0;
            boids[i].move_speed = 0;
            boids[i].m_speed = 0;
        }
        int move = (int)(cos(boids[i].angle) * (boids[i].move_speed));
        boids[i].x += move;
        move = (int)(sin(boids[i].angle) * (boids[i].move_speed));
        boids[i].y += move;
    }
}

void updateBoidBehavior(Boid *boids, Vector2 mousepos, double cohesion)
{
    for (int i = 0; i < numBoids; i++) 
    {
        double angleToMouse = atan2(mousepos.y - boids[i].y, mousepos.x - boids[i].x);
        boids[i].angle = (1 - cohesion) * boids[i].angle + cohesion * angleToMouse;

        double distanceToMouse = sqrt(pow(mousepos.x - boids[i].x, 2) + pow(mousepos.y - boids[i].y, 2));

        boids[i].move_speed = boids[i].move_speed + distanceToMouse/(distanceToMouse + 100);
    }
}

void avoidBoids(Boid *boids, int spacing)
{
    for (int i = 0; i < numBoids; i++) 
    {
        for (int j = 0; j < numBoids; j++) 
        {
            if (i != j) 
            {
                double distance = sqrt(pow(boids[i].x - boids[j].x, 2) + pow(boids[i].y - boids[j].y, 2));
                if (distance < spacing) 
                {
                    double angle = atan2(boids[i].y - boids[j].y, boids[i].x - boids[j].x);
                    boids[i].x -= (int)(cos(angle) * 2);
                    boids[i].y -= (int)(sin(angle) * 2);
                }
            }
        }
    }
}

void updateFollowing(Boid* boids, int cohesion, int speed, int teleport)
{
    for (int i = 0; i < numBoids; i++)
    {
        Vector2 currentBoid = (Vector2){boids[i].x, boids[i].y};
        Vector2 centerofmass = (Vector2){0, 0};
        int count = 0;
        for (int j = 0; j < numBoids; j++)
        {
            if (i != j)
            {
                Vector2 otherBoid = (Vector2){boids[j].x, boids[j].y};
                float distance =  fabs(currentBoid.x - otherBoid.x) + fabs(currentBoid.y - otherBoid.y);
                centerofmass.x += otherBoid.x;
                centerofmass.y += otherBoid.y;
                if (distance < cohesion)
                {
                    boids[i].move_speed = speed; ;
                    // boids[i].speed = clamp(boids[i].speed, 20  , 50);
                    boids[i].angle += atan2(otherBoid.y - currentBoid.y, otherBoid.x - currentBoid.x)/4;
                    count++;
                }
            }
            
        }
        if (count > 0)
        {
        centerofmass.x /= numBoids;
        centerofmass.y /= numBoids;
        boids[i].angle += atan2(centerofmass.y - currentBoid.y, centerofmass.x - currentBoid.x)/4;
        }
        if (teleport)
            boids[i].move_speed = (rand() % 10000) / 50;
        else{
        boids[i].move_speed -= (rand() % 10000) / 5000;
        boids[i].move_speed += (rand() % 10000) / 5000;
        }
        (void)teleport;
        
    }
}

void updateCohesionSpeed(Boid* boids, int cohesion)
{
    for (int i = 0; i < numBoids; i++)
    {
        
        Vector2 centerOfMass = {0, 0};
        int count = 0;
        for (int j = 0; j < numBoids; j++)
        {
            if (i != j)
            {
                Vector2 other = (Vector2){boids[j].x, boids[j].y};
                float distance =  abs(boids[i].x - boids[i].x) + fabs(other.y - other.y);
                if (distance < cohesion)
                {
                    centerOfMass.x += other.x;
                    centerOfMass.y += other.y;
                    count++;
                }
            }
        }
        if (count > 0)
        {
            centerOfMass.x /= count;
            centerOfMass.y /= count;
            boids[i].m_speed += 0.1 * (abs(boids[i].x -boids[i].x) + fabs(centerOfMass.y - centerOfMass.y));
        }
    }
}
/* typedef struct Boid{
    int x;
    int y;
    double move_speed;
    double angle;
    double m_speed;
    double m_angle;
    int species;
    int count;
    int alive;
    Color color;
    Vector2 prevPositions[20];
}Boid;
 */
void Update(Boid boids[], Vector2 mousepos, params p, int teleport)
{
    moveBoids(boids);
   // handleCollision(boids, blocks, numBlocks);

    for (int i = 0; i < numBoids; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            boids[i].prevPositions[j] = boids[i].prevPositions[j + 1];
        }
        boids[i].prevPositions[19] = (Vector2){ boids[i].x, boids[i].y };
    }//myPlayer->x, boids[0].move_speed = 0;
    updateBoidBehavior(boids, mousepos, p.general_behavior); //values between 1 and 2 are most stable
    updateCohesionSpeed(boids, p.cohesion_distance);
    avoidBoids(boids, p.avoid_distance);
    updateFollowing(boids, p.follow_distance, p.follow_move_speed, teleport);


}
