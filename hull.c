#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

typedef struct Vector2 {
    float x;  // Vector x component
    float y;  // Vector y component
} Vector2;

enum { pos, dir };

typedef struct Arrow {
    Vector2 position;
    float size;
    float angle;
} Arrow;

// Hermite interpolation function
Vector2 hermite(Vector2 p0, Vector2 t0, Vector2 p1, Vector2 t1, float t) {
    float tsq = t * t;
    float tcub = tsq * t;
    float blend1 = 2 * tcub - 3 * tsq + 1;
    float blend2 = -2 * tcub + 3 * tsq;
    float blend3 = tcub - 2 * tsq + t;
    float blend4 = tcub - tsq;
    
    Vector2 result;
    result.x = blend1 * p0.x + blend2 * p1.x + blend3 * t0.x + blend4 * t1.x;
    result.y = blend1 * p0.y + blend2 * p1.y + blend3 * t0.y + blend4 * t1.y;
    
    return result;
}

int main(void) {
    SDL_Event event;
    Arrow *arrows = NULL;
    int arrow_count = 0;
    int dragging = 0;
    Vector2 *curve = NULL;
    int curve_count = 0;
    int quit;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Arrows and Curve on Screen", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT)
                quit = 1;
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Add an arrow to the list
                    arrow_count++;
                    arrows = realloc(arrows, sizeof(Arrow) * arrow_count);

                    if (arrows == NULL) {
                        fprintf(stderr, "Failed to reallocate memory for arrows\n");
                        return -1;
                    }

                    arrows[arrow_count - 1].position.x = event.button.x;
                    arrows[arrow_count - 1].position.y = event.button.y;
                    arrows[arrow_count - 1].size = 0;
                    arrows[arrow_count - 1].angle = 0;

                    // Set dragging state
                    dragging = 1;

                    // Add a point to the curve
                    curve_count++;
                    curve = realloc(curve, sizeof(Vector2) * curve_count);

                    if (curve == NULL) {
                        fprintf(stderr, "Failed to reallocate memory for curve\n");
                        return -1;
                    }

                    curve[curve_count - 1].x = event.button.x;
                    curve[curve_count - 1].y = event.button.y;
                }
            } else if (event.type == SDL_MOUSEMOTION) {
                if (dragging) {
                    // Update the arrow size and direction while dragging
                    arrows[arrow_count - 1].size = sqrt(
                        pow(event.motion.x - arrows[arrow_count - 1].position.x, 2) +
                        pow(event.motion.y - arrows[arrow_count - 1].position.y, 2));
                    arrows[arrow_count - 1].angle =
                        atan2(event.motion.y - arrows[arrow_count - 1].position.y,
                              event.motion.x - arrows[arrow_count - 1].position.x);

                    // // Update the curve point
                    // curve[curve_count - 1].x = event.motion.x;
                    // curve[curve_count - 1].y = event.motion.y;

                    // Clear the renderer
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderClear(renderer);

                    // Draw all arrows
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    for (int i = 0; i < arrow_count; i++) {
                        SDL_RenderDrawLine(
                            renderer, arrows[i].position.x +
                                         arrows[i].size * cos(arrows[i].angle),
                            arrows[i].position.y +
                                arrows[i].size * sin(arrows[i].angle),
                            arrows[i].position.x, arrows[i].position.y);
                    }

                    // Draw the curve using Hermite interpolation
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    for (int i = 1; i < curve_count; i++) {
                        Vector2 p0 = curve[i - 1];
                        Vector2 p1 = curve[i];
                        Vector2 t0 = {
                            arrows[i - 1].size * cos(arrows[i - 1].angle),
                            arrows[i - 1].size * sin(arrows[i - 1].angle)};
                        Vector2 t1 = {
                            arrows[i].size * cos(arrows[i].angle),
                            arrows[i].size * sin(arrows[i].angle)};

                        // Draw a line segment of the Hermite curve
                        for (float t = 0.0; t <= 1.0; t += 0.01) {
                            Vector2 point = hermite(p0, t0, p1, t1, t);
                            SDL_RenderDrawPoint(renderer, (int)point.x, (int)point.y);
                        }
                    }

                    // Update the screen
                    SDL_RenderPresent(renderer);
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Reset dragging state
                    dragging = 0;
                }
            }
        }
    }

    free(arrows);  // Free allocated memory
    free(curve);   // Free allocated memory
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
