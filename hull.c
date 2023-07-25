#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

//gcc hull.c -lm -lSDL2
typedef struct Vector2
{
	float x; // Vector x component
	float y; // Vector y component
}				Vector2;

SDL_Window		*window = NULL;
SDL_Renderer	*renderer = NULL;
Vector2 points[1000]; // Array to store the points
int				pointCount = 0;   // Number of points

// Function to swap two points
void	swap(Vector2 *a, Vector2 *b)
{
	Vector2	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

// Function to find the orientation of three points
int	orientation(Vector2 p, Vector2 q, Vector2 r)
{
	int	val;

	val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (val == 0)
		return (0);             // Collinear
	return ((val > 0) ? 1 : 2); // Clockwise or counterclockwise
}

// Function to calculate the square of the distance between two points
int	distSq(Vector2 p1, Vector2 p2)
{
	return (p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y);
}

// Function to compare two points based on polar angle
int	compare(const void *vp1, const void *vp2)
{
	Vector2	*p1;
	Vector2	*p2;
	int		o;

	p1 = (Vector2 *)vp1;
	p2 = (Vector2 *)vp2;
	o = orientation(points[0], *p1, *p2);
	if (o == 0)
		return ((distSq(points[0], *p2) >= distSq(points[0], *p1)) ? -1 : 1);
	return ((o == 2) ? -1 : 1);
}

// Function to remove duplicate points with the same polar angle
int	removeDuplicateAngles(int n)
{
	int	m;

	m = 1;
	for (int i = 1; i < n; i++)
	{
		while (i < n - 1 && orientation(points[0], points[i], points[i
				+ 1]) == 0)
			i++;
		points[m] = points[i];
		m++;
	}
	return (m);
}

// Graham's Scan algorithm
void	convexHull(void)
{
	int		ymin;
	int		y;
	int		m;
	int		top;
	Vector2	stack[1000];
	int		j;

	// Find the bottommost Vector2
	ymin = points[0].y, min = 0;
	for (int i = 1; i < pointCount; i++)
	{
		y = points[i].y;
		// Pick the bottom-most or choose the left-most Vector2 in case of a tie
		if ((y < ymin) || (ymin == y && points[i].x < points[min].x))
			ymin = points[i].y, min = i;
	}
	// Place the bottom-most Vector2 at first position
	swap(&points[0], &points[min]);
	// Sort the remaining points based on polar angle
	qsort(&points[1], pointCount - 1, sizeof(Vector2), compare);
	// Remove points with duplicate polar angles
	m = removeDuplicateAngles(pointCount);
	if (m < 3)
		return ; // Convex hull is not possible
	// Clear the renderer
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	// Draw all points
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	for (int i = 0; i < pointCount; i++)
		SDL_RenderDrawPoint(renderer, points[i].x, points[i].y);
	// Create an empty stack and push the first three points
	top = 2;
	stack[0] = points[0];
	stack[1] = points[1];
	stack[2] = points[2];
	// Process the remaining points
	for (int i = 3; i < m; i++)
	{
		// Keep removing the top while the angle formed by points[i],
			top of the stack, and the second top does not make a left turn
		while (orientation(stack[top - 1], stack[top], points[i]) != 2)
			top--;
		stack[++top] = points[i];
	}
	// Draw the lines of the convex hull
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int i = 0; i <= top; i++)
	{
		j = (i + 1) % (top + 1);
		SDL_RenderDrawLine(renderer, stack[i].x, stack[i].y, stack[j].x,
				stack[j].y);
	}
	//Draw every points as crosses
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	for (int i = 0; i < pointCount; i++)
	{
		SDL_RenderDrawLine(renderer, points[i].x - 5, points[i].y - 5,
				points[i].x + 5, points[i].y + 5);
		SDL_RenderDrawLine(renderer, points[i].x - 5, points[i].y + 5,
				points[i].x + 5, points[i].y - 5);
	}
	// Update the screen
	SDL_RenderPresent(renderer);
}

int	main(void)
{
	SDL_Event	event;
	int			quit;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return (-1);
	}
	window = SDL_CreateWindow("Convex Hull", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return (-1);
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL_Error: %s\n",
				SDL_GetError());
		return (-1);
	}
	quit = 0;
	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
				quit = 1;
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					// Add a Vector2 at the clicked position
					points[pointCount].x = event.button.x;
					points[pointCount].y = event.button.y;
					pointCount++;
					// Draw the new Vector2
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					SDL_RenderDrawPoint(renderer, event.button.x,
							event.button.y);
					// Compute and draw the convex hull
					convexHull();
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					// Clear the points
					pointCount = 0;
					// Clear the renderer
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					SDL_RenderClear(renderer);
					// Update the screen
					SDL_RenderPresent(renderer);
				}
			}
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return (0);
}
