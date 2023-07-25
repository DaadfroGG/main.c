#include <boids.h>

// Function to swap two boids
Vector2 points[numBoids * 20 + 1];

void swap(Vector2* a, Vector2* b) {
    Vector2 temp = *a;
    *a = *b;
    *b = temp;
}

// Function to find the orientation of three points
double orientation(Vector2 p, Vector2 q, Vector2 r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0)
        return 0;  // Collinear
    return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
}

// Function to calculate the square of the distance between two points
double distSq(Vector2 p1, Vector2 p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) +
           (p1.y - p2.y) * (p1.y - p2.y);
}

// Function to compare two points based on polar angle
int compare(const void* vp1, const void* vp2) {
    Vector2* p1 = (Vector2*)vp1;
    Vector2* p2 = (Vector2*)vp2;

    int o = orientation(points[0], *p1, *p2);
    if (o == 0)
        return (distSq(points[0], *p2) >= distSq(points[0], *p1)) ? -1 : 1;

    return (o == 2) ? -1 : 1;
}


// Function to remove duplicate points with the same polar angle
double removeDuplicateAngles(Vector2 points[], int n) {
    int m = 1;
    for (int i = 1; i < n; i++) {
        while (i < n - 1 && orientation(points[0], points[i], points[i + 1]) == 0)
            i++;
        points[m] = points[i];
        m++;
    }
    return m;
}
/* typedef struct player
{
    Vector2 position;
    Vector2 speed;
    int size;
    int in_the_end;//linkin park
    int grounded;
    int timer;
    int score;
    int end_time;
    int clock;
    double gravity;
    Vector2 shell[numBoids * 20];
    int shell_count;

}player;
 */
// Graham's Scan algorithm

void convexHull(Boid boids[], player myPlayer[]) {
    int numPoints = numBoids * 20 + 1; // Increase the number of points

    for (int i = 0; i < numBoids; i++) {
        if (boids[i].alive == 0) {
                    points[i].x = myPlayer->position.x;
                    points[i].y = myPlayer->position.y;
                for (int j = 0; j < 20; j++) {
                    points[i * 20 + j].x = myPlayer->position.x;
                    points[i * 20 + j].y = myPlayer->position.y;
        }
        }
        else{
            for (int j = 0; j < 20; j++) {
                points[i * 20 + j].x = boids[i].prevPositions[j].x;
                points[i * 20 + j].y = boids[i].prevPositions[j].y;
            }
        }
        }
    

    // Add player position to the points array
    points[numBoids * 20].x = myPlayer->position.x;
    points[numBoids * 20].y = myPlayer->position.y;

    // Find the bottommost Vector2
    int ymin = points[0].y, min = 0;
    for (int i = 1; i < numPoints; i++) {
        int y = points[i].y;

        // Pick the bottom-most or choose the left-most Vector2 in case of a tie
        if ((y < ymin) || (ymin == y && points[i].x < points[min].x))
            ymin = points[i].y, min = i;
    }

    // Place the bottom-most Vector2 at first position
    swap(&points[0], &points[min]);

    // Sort the remaining points based on polar angle
    qsort(&points[1], numPoints - 1, sizeof(Vector2), compare);

    // Remove points with duplicate polar angles
    int m = removeDuplicateAngles(points, numPoints);

    if (m < 3) return;  // Convex hull is not possible

    // Create an empty stack and push the first three points
    int top = 2;
    myPlayer->shell[0] = points[0];
    myPlayer->shell[1] = points[1];
    myPlayer->shell[2] = points[2];

    // Process the remaining points
    for (int i = 3; i < m; i++) {
        // Keep removing the top while the angle formed by points[i], top of the stack, and the second top does not make a left turn
        while (orientation(myPlayer->shell[top - 1], myPlayer->shell[top], points[i]) != 2)
            top--;
        myPlayer->shell[++top] = points[i];
    }
    //set myplayer shell count
    myPlayer->shell_count = top;
}
