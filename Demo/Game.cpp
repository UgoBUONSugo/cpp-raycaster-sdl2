#include <math.h>
#include <iostream>
#include "Game.h"
#define PI 3.1415926535
#define PI2 PI/2
#define PI3 3 * PI/2

const int thickness = 15;
float px, py, pdx, pdy, pa;

int mapX = 8, mapY = 8, mapS = 64;
const int map[] =
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,1,0,1,
	1,0,0,1,0,1,0,1,
	1,0,0,1,1,1,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1
};

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
{

}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		512,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	//Initialize player position and angle
	px = 300; py = 300; pdx = cos(pa)*5; pdy = sin(pa)*5;

	return true;
}

void Game::RayCaster() {
	int r, mx, my, mp, dof; float rx, ry, ra, xo, yo;
	ra = pa;
	for(r = 0; r < 1; r++)
	{
		//Check for horizontal lines
		/*dof = 0;
		float aTan = -1 / tan(ra);
		if (ra > PI) { ry = (((int)py >> 6) << 6) - 0.0001; rx = (py - ry) * aTan + px; yo = -64; xo = -yo * aTan; } //looking up
		if (ra < PI) { ry = (((int)py >> 6) << 6) + 64; rx = (py - ry) * aTan + px; yo = 64; xo = yo * aTan; } //looking down
		if (ra == 0 || ra == PI) { rx = px; ry = py; dof = 8; }
		while(dof < 8)
		{
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
			if (mp < mapX * mapY && map[mp] == 1) { dof = 8; } //Hit wall
			else { rx += xo; ry += yo; dof += 1; } //Next line
		}

		SDL_RenderDrawLine(mRenderer, px, py, rx, ry);*/

		//Check for vertical lines
		dof = 0;
		float nTan = -tan(ra);
		rx = px;
		if (ra > PI2 && ra < PI3) { rx = (((int)px >> 6) << 6) - 0.0001; ry = (px - rx) * nTan + py; xo = -64; yo = -xo * nTan; } //looking left
		if (ra < PI2 || ra > PI3) { ry = (((int)py >> 6) << 6) + 64; rx = (px - rx) * nTan + py; yo = 64; yo = -xo * nTan; } //looking right
		if (ra == 0 || ra == PI) { rx = px; ry = py; dof = 8; }
		while (dof < 8)
		{
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
			if (mp < mapX * mapY && map[mp] == 1) { dof = 8; } //Hit wall
			else { rx += xo; ry += yo; dof += 1; } //Next line
		}

		SDL_RenderDrawLine(mRenderer, px, py, rx, ry);
	}
}

void Game::DrawMap() {
	int x, y, x0, y0;
	for (y = 0; y < mapY; y++) {
		for (x = 0; x < mapX; x++) {      //Wall Color												//Floor Color 
			if (map[y * mapX + x] == 1) { SDL_SetRenderDrawColor(mRenderer, 0, 155, 255, 255); } else { SDL_SetRenderDrawColor(mRenderer, 110, 110, 110, 255); }
			x0 = x * mapS; y0 = y * mapS;
			SDL_Rect wall = { x0 +2, y0 +2, mapS -2, mapS -2};
			SDL_RenderFillRect(mRenderer, &wall);
		}
	}
}

void Game::DrawPlayer() {
	//Set player color
	SDL_SetRenderDrawColor(
		mRenderer,
		255, // R
		255, // G
		60, // B
		255 // A
	);

	//Draw player
	SDL_Rect player{
		px,
		py,
		12,
		12
	};
	SDL_RenderFillRect(mRenderer, &player);

	SDL_SetRenderDrawColor(
		mRenderer,
		255, // R
		0, // G
		0, // B
		255 // A
	);
	SDL_RenderDrawLine(mRenderer, px, py, px+pdx*5, py+pdy*5);
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		//UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		// If we get an SDL_QUIT event, end loop
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	else if (state[SDL_SCANCODE_W])
	{
		px += pdx; py += pdy;
	}
	else if (state[SDL_SCANCODE_S])
	{
		px -= pdx; py -= pdy;
	}

	else if (state[SDL_SCANCODE_D])
	{
		pa += 0.1; if (pa > 2*PI) { pa -= 2 * PI; } pdx = cos(pa)*5; pdy = sin(pa)*5;
	}
	else if (state[SDL_SCANCODE_A])
	{
		pa -= 0.1; if (pa < 0) {pa += 2 * PI;} pdx = cos(pa)*5; pdy = sin(pa)*5;
	}
}

void Game::UpdateGame()
{
	
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(
		mRenderer,
		60, // R
		60, // G
		60, // B
		255 // A
	);

	// Clear back buffer
	SDL_RenderClear(mRenderer);

	DrawMap();
	DrawPlayer();
	RayCaster();

	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
