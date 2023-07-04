#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
// gcc editor.c -lm -lSDL2
typedef struct Vector2 {
    float x;                // Vector x component
    float y;                // Vector y component
} Vector2;

// Rectangle, 4 components
typedef struct Rectangle {
    float x;                // Rectangle top-left corner position x
    float y;                // Rectangle top-left corner position y
    float width;            // Rectangle width
    float height;           // Rectangle height
} Rectangle;

typedef struct Block {
    Vector2 position;
    Vector2 size;
    Rectangle rect;
} Block;

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Maximum number of rectangles
const int MAX_RECTANGLES = 100;
// ...

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Map Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Failed to create SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Array to store rectangles
    Block blocks[MAX_RECTANGLES];
    int numRectangles = 0;

    // Mouse state
    bool isDrawing = false;
    Vector2 startPos;

    // Map position and zoom
    Vector2 mapPosition = {0.0f, 0.0f};
    float mapZoom = 1.0f;

    // Main loop
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        isDrawing = true;
                        startPos.x = (float) event.button.x / mapZoom - mapPosition.x;
                        startPos.y = (float) event.button.y / mapZoom - mapPosition.y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (isDrawing) {
                            // Calculate rectangle properties
                            float width = (float) event.button.x / mapZoom - mapPosition.x - startPos.x;
                            float height = (float) event.button.y / mapZoom - mapPosition.y - startPos.y;

                            // Add rectangle to the array
                            Block block;
                            block.position.x = startPos.x;
                            block.position.y = startPos.y;
                            block.size.x = width;
                            block.size.y = height;
                            block.rect.x = startPos.x + mapPosition.x;
                            block.rect.y = startPos.y + mapPosition.y;
                            block.rect.width = width;
                            block.rect.height = height;
                            blocks[numRectangles++] = block;

                            // Stop drawing
                            isDrawing = false;
                        }
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        // Zoom in
                        mapZoom *= 1.1f;
                    } else if (event.wheel.y < 0) {
                        // Zoom out
                        mapZoom /= 1.1f;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (event.motion.state & SDL_BUTTON_RMASK) {
                        // Move the map
                        mapPosition.x -= (float) event.motion.xrel / mapZoom;
                        mapPosition.y -= (float) event.motion.yrel / mapZoom;
                    }
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_s:
                            // Move map up
                            mapPosition.y += 10.0f / mapZoom;
                            break;
                        case SDLK_w:
                            // Move map down
                            mapPosition.y -= 10.0f / mapZoom;
                            break;
                        case SDLK_d:
                            // Move map left
                            mapPosition.x += 10.0f / mapZoom;
                            break;
                        case SDLK_a:
                            // Move map right
                            mapPosition.x -= 10.0f / mapZoom;
                            break;
                        }
                        if (mapPosition.y < 0.0f)
                            mapPosition.y = 0.0f;
                        if (mapPosition.x < 0.0f)
                            mapPosition.x = 0.0f;
                    break;
            }
        }

        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the rectangles
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < numRectangles; i++) {
            SDL_Rect rect = {
                    (int) ((blocks[i].rect.x - mapPosition.x) * mapZoom),
                    (int) ((blocks[i].rect.y - mapPosition.y) * mapZoom),
                    (int) (blocks[i].rect.width * mapZoom),
                    (int) (blocks[i].rect.height * mapZoom)
            };
            SDL_RenderDrawRect(renderer, &rect);
        }

        // Draw the currently drawing rectangle
        if (isDrawing) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            SDL_Rect rect = {
                    (int) (startPos.x * mapZoom + mapPosition.x * mapZoom),
                    (int) (startPos.y * mapZoom + mapPosition.y * mapZoom),
                    (int) ((mouseX / mapZoom - mapPosition.x - startPos.x) * mapZoom),
                    (int) ((mouseY / mapZoom - mapPosition.y - startPos.y) * mapZoom)
            };
            SDL_RenderDrawRect(renderer, &rect);
        }

        // Present the renderer
        SDL_RenderPresent(renderer);
    }

    // Cleanup and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Print the coordinates of the rectangles
    printf("Rectangles:\n");
    for (int i = 0; i < numRectangles; i++) {
        printf("Rectangle %d:\n", i + 1);
        printf("    Top-left corner position: (%.2f, %.2f)\n", blocks[i].rect.x, blocks[i].rect.y);
        printf("    Width: %.2f\n", blocks[i].rect.width);
        printf("    Height: %.2f\n", blocks[i].rect.height);
        printf("\n");
    }

    return 0;
}
