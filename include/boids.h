#ifndef BOIDS_H
#define BOIDS_H

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_test_font.h>


#define numBoids  200// 700 is big for some reasons 27 is the min cap?
#define screenHeight  2000
#define screenWidth  3000

#define FPS 60
#define FRAME_DELAY (1000 / FPS)
/* 
#if defined(__cplusplus)
    #define CLITERAL(type)      type
#else
    #define CLITERAL(type)      (type)
#endif
 */

enum {true, false} ;
// Vector2, 2 components
typedef struct Vector2 {
    float x;                // Vector x component
    float y;                // Vector y component
} Vector2;

// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} Color;

// Rectangle, 4 components
typedef struct Rectangle {
    float x;                // Rectangle top-left corner position x
    float y;                // Rectangle top-left corner position y
    float width;            // Rectangle width
    float height;           // Rectangle height
} Rectangle;

typedef struct Block
{
    Vector2 position;
    Vector2 size;
    Rectangle rect;
    int type;
} Block;

enum e_type
{
    hitbox,
    boidbox,
    end
};

typedef struct HitBlock
{
    Vector2 position;
    Vector2 size;
    Rectangle rect;
    int hit;
    int capacity;
    double permuability;
    double absorbtion;
    int type;
} HitBlock;

typedef struct Boid{
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

typedef struct player
{
    Vector2 position;
    Vector2 speed;
    int size;
    int in_the_end;//linkin park
    int grounded;
    int is_in_block;
    int timer;
    int score;
    int end_time;
    int clock;
    double gravity;
    Vector2 shell[numBoids * 20];
    int shell_count;
    int camera_type;

}player;

typedef struct boidParameters
{
    double general_behavior;
    int cohesion_distance;
    int avoid_distance;
    int follow_distance;
    int follow_move_speed;
}params;



void Update(Boid boids[], Vector2 mousepos, params p, int teleport);
unsigned short *get_screen_size(void);

Color colorBoids(Boid* boid);
/*     color.r = (int)(sin(timing * 2 + boid->x * 0.01) * 127 + 128);
    color.g = (int)(sin(timing * 2 + boid->y * 0.01) * 127 + 128);
    color.b = (int)(sin(timing * 2 + boid->x * 0.01 + boid->y * 0.01) * 127 + 128);
    color.a = 255; */

float clamp(float value, float min, float max);

float mapValue(float value, float inputMin, float inputMax, float outputMin, float outputMax);

Vector2 subtractvect(Vector2 a, Vector2 b);

void Draw(player myPlayer[], Boid boids[], Block blocks[], int numBlocks, SDL_Renderer *renderer, HitBlock countBlocks[], int numhitbox, int cameraX, int cameraY, params p[], double zoom, int pause);
void Control(Vector2* mousepos, int* teleport, Vector2* teleportPos, player* myPlayer, params* p, int cameraX, int cameraY, Boid boids[], double zoom, Block blocks[], int numBlocks);
void  Physics(player* myPlayer, Block blocks[], HitBlock countBlock[], int numBlocks, Boid boids[]);
void BoidCollision(Boid boids[], HitBlock countBox[], int numBlocks, player myPlayer[]);

void Movement(player* myPlayer);



void InitBoids(Boid boids[], player myPlayer[]);
void initPLayer(player *myPlayer, params *p, Vector2 *prevPos, Vector2 *teleportPos, int *teleport);
void InitBlocks(Block *blocks,HitBlock *countBox, int numBlocks[], player Myplayer[]);

void convexHull(Boid boids[], player myPlayer[]);
#endif
