#include <SDL2/SDL.h>
#include <SDL2/SDL_test_font.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
//gcc editor.c -lSDL2 -lSDL2_test -lm -o editor
#define screenHeight 900
#define screenWidth 1800

#define FPS 120
#define FRAME_DELAY (1000 / FPS)
enum
{
	true,
	false
};
typedef struct Vector2
{
	float x; // Vector x component
	float y; // Vector y component
}				Vector2;

enum			e_type
{
	hitbox,
	boidbox,
	end,
	start
};
// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color
{
	unsigned char r; // Color red value
	unsigned char g; // Color green value
	unsigned char b; // Color blue value
	unsigned char a; // Color alpha value
}				Color;

// Rectangle, 4 components
typedef struct Rectangle
{
	float x;      // Rectangle top-left corner position x
	float y;      // Rectangle top-left corner position y
	float width;  // Rectangle width
	float height; // Rectangle height
}				Rectangle;

typedef struct Block
{
	Vector2		position;
	Vector2		size;
	Rectangle	rect;
	int			type;
}				Block;

#define MAX_BLOCKS 100

Block			blocks[MAX_BLOCKS];
int				blockCount = 0;
Vector2			playerPos = {0};

void	SDL_RenderDrawCircle(SDL_Renderer *renderer, int x, int y, int radius,
		SDL_Color color)
{
	double	dx;

	for (double dy = 1; dy <= radius; dy += 1.0)
	{
		dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(renderer, x - dx, y + dy - radius, x + dx, y + dy
				- radius);
		SDL_RenderDrawLine(renderer, x - dx, y - dy + radius, x + dx, y - dy
				+ radius);
	}
}

void	InitBlocksFromFile(Block blocks[], int numBlocks[],
		const char *filename)
{
	FILE	*file;
	char	line[256];
	int		lineCount;
	int		blockIndex;
		int type;

	file = fopen(filename, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Failed to open file: %s\n", filename);
		return ;
	}
	lineCount = 0;
	blockIndex = 0;
	if (fgets(line, sizeof(line), file) != NULL)
	{
		lineCount++;
		float playerX, playerY;
		if (sscanf(line, "%f,%f", &playerX, &playerY) == 2)
		{
			playerPos.x = playerX;
			playerPos.y = playerY;
		}
		else
		{
			fprintf(stderr, "Invalid player starting position format in file
					%s at line %d\n", filename, lineCount);
		}
	}
	while (fgets(line, sizeof(line), file) != NULL)
	{
		lineCount++;
		// Ignore empty lines and comments starting with '#'
		if (line[0] == '\n' || line[0] == '#')
			continue ;
		// Parse rectangle parameters from the line
		float x, y, width, height;
		if (sscanf(line, "%f, %f, %f, %f, %d", &x, &y, &width, &height,
				&type) != 5)
		{
			fprintf(stderr, "Invalid rectangle format in file %s at line %d\n",
					filename, lineCount);
			continue ;
		}
		// Initialize the block and countBox with the parsed parameters
		blocks[blockIndex].position = (Vector2){x, y};
		blocks[blockIndex].size = (Vector2){width, height};
		blocks[blockIndex].rect = (Rectangle){x, y, width, height};
		blocks[blockIndex].type = type;
		/* countBox[blockIndex].capacity = numBoids / 2;
    countBox[blockIndex].permuability = 1;
     */
		blockIndex++;
	}
	numBlocks[0] = blockIndex;
	fclose(file);
}

void	OutputBlocktoFile(Block blocks[], int numBlocks, const char *filename)
{
	FILE	*file;

	file = fopen(filename, "w+");
	if (file == NULL)
	{
		fprintf(stderr, "Failed to open file: %s\n", filename);
		return ;
	}
	// Clear the file by truncating its content
	freopen(filename, "w", file);
	// Write the player starting position to the file
	fprintf(file, "%.2f,%.2f\n", playerPos.x, playerPos.y);
	for (int i = 0; i < numBlocks; i++)
	{
		// Write each block to the file
		fprintf(file, "%.2f,%.2f,%.2f,%.2f,%d\n", blocks[i].position.x,
				blocks[i].position.y, blocks[i].size.x, blocks[i].size.y,
				blocks[i].type);
	}
	fclose(file);
}

int	main(int ac, char *av[])
{
	printf("you can load a map by passing the file name as an argument\nIf no argument is passed,
			a new map will be created\nYou can specify the output file name by passing it as a second argument\nIf no argument is passed,
			the map will be saved as \"level.txt\"\n");
	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	// Create a window
	SDL_Window *window = SDL_CreateWindow("main.c - map editor",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth,
			screenHeight, SDL_WINDOW_SHOWN);
	// Create a renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
			SDL_RENDERER_ACCELERATED);
	//Initialize variables
	Vector2 mousepos = {-100.0f, -100.0f};
	if (ac >= 2)
	{
		int numBlocks[1];
		InitBlocksFromFile(blocks, numBlocks, av[1]);
		blockCount = numBlocks[0];
	}
	// Game loop
	short quit = false;
	Uint32 frameStart;
	int frameTime;
	double zoom = 0.8;
	int camera_type = 0;
	double endX = 0;
	double endY = 0;
	double startX = 0;
	double startY = 0;
	SDL_Color cursorColor = {255, 255, 255, 255};
	int blockType = 0;

	Vector2 mapPosition = playerPos;
	int dragging = false;
	while (quit)
	{
		static int cameraX = 0;
		static int cameraY = 0;
		frameStart = SDL_GetTicks();

		// Calculate target camera position based on player position and zoom
		int targetCameraX = mapPosition.x - (screenWidth / 2) / zoom;
		int targetCameraY = mapPosition.y - (screenHeight / 2) / zoom;

		// Apply "drag" effect to gradually move the camera towards the target position
		const float cameraDrag = 0.05;
			// Adjust the drag value for desired effect
		cameraX += (targetCameraX - cameraX) * cameraDrag;
		cameraY += (targetCameraY - cameraY) * cameraDrag;
		int mouseX = 0;
		int mouseY = 0;
		SDL_GetMouseState(&mouseX, &mouseY);
		// convert mouse position to world position (using map position and camera position)
		Vector2 worldMousePos = {(float)mouseX / zoom + cameraX, (float)mouseY
			/ zoom + cameraY};

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				if (event.key.keysym.sym == SDLK_q)
				{
					blockType++;
					blockType %= 4;
				}
				if (event.key.keysym.sym == SDLK_e)
				{
					blockType--;
					if (blockType < 0)
						blockType = 3;
				}
			}
			if (event.type == SDL_MOUSEMOTION)
			{
				if (event.motion.state & SDL_BUTTON_RMASK)
				{
					// Move the map
					mapPosition.x -= (float)event.motion.xrel / zoom;
					mapPosition.y -= (float)event.motion.yrel / zoom;
				}
			}

			if (event.type == SDL_MOUSEMOTION)
			{
				if (dragging)
				{
					// Update the ending position of the rectangle while dragging
					endX = event.motion.x;
					endY = event.motion.y;
				}
			}
			if (event.type == SDL_MOUSEWHEEL)
			{
				if (event.wheel.y > 0) // scroll up
				{
					//make the zoom less and less effective as it gets bigger (max zoom 1.0)
					if (zoom < 1.0)
						zoom += 0.01;
					//recalculate camera position
					targetCameraX = (mapPosition.x) - (screenWidth / 2) / zoom;
					targetCameraY = (mapPosition.y) - (screenHeight / 2) / zoom;

					cameraX += (targetCameraX - cameraX);
					cameraY += (targetCameraY - cameraY);
				}
				else if (event.wheel.y < 0) // scroll down
				{
					//make the zoom less and less effective as it gets bigger
					if (zoom > 0.01)
						zoom -= 0.01;
					//recalculate camera position
					targetCameraX = (mapPosition.x) - (screenWidth / 2) / zoom;
					targetCameraY = (mapPosition.y) - (screenHeight / 2) / zoom;
					cameraX += (targetCameraX - cameraX);
					cameraY += (targetCameraY - cameraY);
				}
			}

			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT && blockType != 3)
				{
					// Start dragging
					startX = worldMousePos.x;
					startY = worldMousePos.y;
					dragging = true;
				}
				else if (event.button.button == SDL_BUTTON_LEFT
						&& blockType == 3)
				{
					// place starting position
					playerPos.x = worldMousePos.x;
					playerPos.y = worldMousePos.y;
					// playerPos.x = worldMousePos.x + (mapPosition.x * zoom);
					// playerPos.y = worldMousePos.y + (mapPosition.y * zoom);
				}
				else if (event.button.button == SDL_BUTTON_MIDDLE)
				{
					// Remove a rectangle if clicked with the scroll button
					Vector2 clickPos = {worldMousePos.x, worldMousePos.y};
					for (int i = 0; i < blockCount; i++)
					{
						if (clickPos.x >= blocks[i].position.x
							&& clickPos.x <= blocks[i].position.x
							+ blocks[i].size.x &&
							clickPos.y >= blocks[i].position.y
								&& clickPos.y <= blocks[i].position.y
								+ blocks[i].size.y)
						{
							// Remove the rectangle by shifting the subsequent blocks and reducing the blockCount
							for (int j = i; j < blockCount - 1; j++)
							{
								blocks[j] = blocks[j + 1];
							}
							blockCount--;
							break ;
								// Break the loop since we found and removed the rectangle
						}
					}
				}
			}

			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				if (event.button.button == SDL_BUTTON_LEFT && blockType != 3)
				{
					// End dragging
					dragging = false;
					endX = worldMousePos.x;
					endY = worldMousePos.y;
					// Create a rectangle based on the starting and ending positions (if the values are negative,
							offset them to the left/top of the rectangle)
					Block newBlock;
					newBlock.position.x = fmin(startX, endX);
					newBlock.position.y = fmin(startY, endY);
					newBlock.size.x = fabs(endX - startX);
					newBlock.size.y = fabs(endY - startY);
					newBlock.rect.x = newBlock.position.x;
					newBlock.rect.y = newBlock.position.y;
					newBlock.rect.width = newBlock.size.x;
					newBlock.rect.height = newBlock.size.y;
					newBlock.type = blockType;

					// Add the new block to the blocks array
					if (blockCount < MAX_BLOCKS)
					{
						blocks[blockCount] = newBlock;
						blockCount++;
					}
				}
			}
		}
		// Get mouse position

		// Clear the screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//zoom unzoom with mouse wheel

		// Move camera...
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_W])
		{
			mapPosition.y -= 10 / zoom;
		}
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_S])
		{
			mapPosition.y += 10 / zoom;
		}
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_A])
		{
			mapPosition.x -= 10 / zoom;
		}
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_D])
		{
			mapPosition.x += 10 / zoom;
		}

		SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // Set grid color
		double gridSize = 1000;
		int startGridX = cameraX / gridSize;
		int startGridY = cameraY / gridSize;
		int endGridX = startGridX + screenWidth;
		int endGridY = startGridY + screenHeight;

		for (int i = startGridX; i <= endGridX; i++)
		{
			SDL_RenderDrawLine(renderer, (i * gridSize - cameraX) * zoom,
					-cameraY * zoom, (i * gridSize - cameraX) * zoom, (endGridY
						* gridSize - cameraY) * zoom);
		}


		for (int i = startGridY; i <= endGridY; i++)
		{
			SDL_RenderDrawLine(renderer, -cameraX * zoom, (i * gridSize
						- cameraY) * zoom, (endGridX * gridSize - cameraX)
					* zoom, (i * gridSize - cameraY) * zoom);
		}
		// Draw the rectangles
		for (int i = 0; i < blockCount; i++)
		{
			SDL_Color color;
			if (blocks[i].type == hitbox)
				color = (SDL_Color){255, 0, 0, 255};
			else if (blocks[i].type == boidbox)
				color = (SDL_Color){0, 0, 255, 255};
			else if (blocks[i].type == end)
				color = (SDL_Color){0, 255, 0, 255};
			SDL_Rect rect = {
				(blocks[i].rect.x - cameraX) * zoom,
				(blocks[i].rect.y - cameraY) * zoom,
				blocks[i].rect.width * zoom,
				blocks[i].rect.height * zoom};
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
					color.a);
			SDL_RenderFillRect(renderer, &rect);
		}
		//draw player

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		//draw cross where player is based on camera position and zoom
		Vector2 playerPosDrawn;
		playerPosDrawn.x = (playerPos.x - cameraX) * zoom;
		playerPosDrawn.y = (playerPos.y - cameraY) * zoom;
		SDL_RenderDrawLine(renderer, playerPosDrawn.x - 10, playerPosDrawn.y
				- 10, playerPosDrawn.x + 10, playerPosDrawn.y + 10);
		SDL_RenderDrawLine(renderer, playerPosDrawn.x + 10, playerPosDrawn.y
				- 10, playerPosDrawn.x - 10, playerPosDrawn.y + 10);

		//draw mouse position
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		//convert world position to screen position
		Vector2 mouse_circle;

		if (blockType == hitbox)
			cursorColor = (SDL_Color){255, 0, 0, 255};
		else if (blockType == boidbox)
			cursorColor = (SDL_Color){0, 0, 255, 255};
		else if (blockType == end)
			cursorColor = (SDL_Color){0, 255, 0, 255};
		else if (blockType == start)
			cursorColor = (SDL_Color){255, 255, 255, 255};
		mouse_circle.x = (worldMousePos.x - cameraX) * zoom;
		mouse_circle.y = (worldMousePos.y - cameraY) * zoom;

		if (blockType != 3)
			SDL_RenderDrawCircle(renderer, mouse_circle.x, mouse_circle.y, 10,
					cursorColor);
		else //draw cursor as a cross
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderDrawLine(renderer, mouse_circle.x - 10, mouse_circle.y
					- 10, mouse_circle.x + 10, mouse_circle.y + 10);
			SDL_RenderDrawLine(renderer, mouse_circle.x + 10, mouse_circle.y
					- 10, mouse_circle.x - 10, mouse_circle.y + 10);
		}
		//write controls (no \n allowed)
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		char message[200];

		sprintf(message, "Controls:");
		SDLTest_DrawString(renderer, 10, 10, message);
		sprintf(message, "Drag right click or use WASD to move camera around");
		SDLTest_DrawString(renderer, 10, 20, message);
		sprintf(message, "Scroll to zoom");
		SDLTest_DrawString(renderer, 10, 30, message);
		sprintf(message, "Drag left click to place a rectangle");
		SDLTest_DrawString(renderer, 10, 40, message);
		sprintf(message, "Arrow keys to move camera");
		SDLTest_DrawString(renderer, 10, 50, message);
		sprintf(message, "Q and E to change block type");
		SDLTest_DrawString(renderer, 10, 60, message);
		sprintf(message, "Click scroll to delete block");
		SDLTest_DrawString(renderer, 10, 70, message);
		sprintf(message, "Press 3 to place beggining position");
		SDLTest_DrawString(renderer, 10, 80, message);
		sprintf(message, "Red == hitbox, blue == boidbox, green == end");
		SDLTest_DrawString(renderer, 10, 90, message);

		// Swap buffers

		SDL_RenderPresent(renderer);
		SDL_FlushEvent(SDL_MOUSEMOTION);
		frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < FRAME_DELAY)
		{
			SDL_Delay(FRAME_DELAY - frameTime);
		}
	}
	// Cleanup and quit SDL
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	//print the rectangle list
	for (int i = 0; i < blockCount; i++)
	{
		printf("%f,%f,%f,%f,%d\n", blocks[i].position.x, blocks[i].position.y,
				blocks[i].size.x, blocks[i].size.y, blocks[i].type);
	}
	if (ac == 2)
		OutputBlocktoFile(blocks, blockCount, av[1]);
	else if (ac == 3)
		OutputBlocktoFile(blocks, blockCount, av[2]);
	else
		OutputBlocktoFile(blocks, blockCount, "level.txt");
	return (0);
}