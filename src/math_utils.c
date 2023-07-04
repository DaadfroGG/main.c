#include <boids.h>

float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float mapValue(float value, float inputMin, float inputMax, float outputMin, float outputMax)
{
    return (value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin;
}

Vector2 subtractvect(Vector2 a, Vector2 b)
{
    Vector2 c = { a.x - b.x, a.y - b.y };
    return c;
}