#include <boids.h>

// Path: src/utils.c

double fast_sqrt(double x)
{
    union {
        int i;
        double x;
    } u;
    u.x = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
    return u.x;
}

double get_dist(double a, double b)//returns the distance between two points, a and b
{
    return (fast_sqrt(a * a + b * b));
}

void moveBoids(Boid *boids, int i)
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

void updateBoidBehavior(Boid *boids, Vector2 mousepos, double cohesion, int i)
{
    double angleToMouse = atan2(mousepos.y - boids[i].y, mousepos.x - boids[i].x);
    boids[i].angle = (1 - cohesion) * boids[i].angle + cohesion * angleToMouse;

    double distanceToMouse = get_dist(mousepos.x - boids[i].x, mousepos.y - boids[i].y);
    // sqrt(pow(mousepos.x - boids[i].x, 2) + pow(mousepos.y - boids[i].y, 2));

    boids[i].move_speed = boids[i].move_speed + distanceToMouse/(distanceToMouse + 100);
}


void avoidBoids(Boid *boids, int spacing, int i)
{
    for (int j = 0; j < numBoids; j++) 
    {
        if (i != j) 
        {
            double distance = get_dist(boids[i].x - boids[j].x, boids[i].y - boids[j].y);
            if (distance < spacing) 
            {
                double angle = atan2(boids[i].y - boids[j].y, boids[i].x - boids[j].x);
                boids[i].x -= (int)(cos(angle) * 2);
                boids[i].y -= (int)(sin(angle) * 2);
            }
        }
    }
}

void updateFollowing(Boid* boids, int cohesion, int speed, int teleport, int i)
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
    centerofmass.x *= numBoidsinverse;
    centerofmass.y *= numBoidsinverse;
    boids[i].angle += atan2(centerofmass.y - currentBoid.y, centerofmass.x - currentBoid.x)/4;
    }
    if (teleport)
        boids[i].move_speed = (rand() % 10000) * 0.025;
    else{
    boids[i].move_speed -= (rand() % 10000) * 0.0005;
    boids[i].move_speed += (rand() % 10000) * 0.0005;
    }
    (void)teleport;
}

void updateCohesionSpeed(Boid* boids, int cohesion, int i)
{

    Vector2 centerOfMass = {0, 0};
    int count = 0;
    for (int j = 0; j < numBoids; j++)
    {
        if (i != j)
        {
            Vector2 other = (Vector2){boids[i].x, boids[i].y};
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

void Update(Boid boids[], Vector2 mousepos, params p, int teleport, Game *game)
{
    for (int i = 0; i < numBoids; i++)
    {
        moveBoids(boids, i);
        for (int j = 0; j < 19; j++)
        {
            boids[i].prevPositions[j] = boids[i].prevPositions[j + 1];
        }
        boids[i].prevPositions[19] = (Vector2){ boids[i].x, boids[i].y };
        updateBoidBehavior(boids, mousepos, p.general_behavior, i); //values between 1 and 2 are most stable
        updateCohesionSpeed(boids, p.cohesion_distance, i);
        avoidBoids(boids, p.avoid_distance, i);
        updateFollowing(boids, p.follow_distance, p.follow_move_speed, teleport, i);
    }
    convexHull(game->boids, game->player);
}
