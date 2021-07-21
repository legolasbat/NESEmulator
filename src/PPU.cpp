#include "PPU.h"

#include <iostream>
#include <bitset>
#include <windows.h>

void PPU::ConnectMemory(Memory *mem) {
	memory = mem;
}

void PPU::PrintTable() {
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	SDL_Surface* surface;

	SDL_Renderer* renderer;
	SDL_Texture* texture;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	//Create window
	window = SDL_CreateWindow("NESEmulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	SDL_SetWindowSize(window, SCREEN_WIDTH*ZOOM, SCREEN_HEIGHT*ZOOM);
	SDL_SetWindowResizable(window, SDL_TRUE);

	//Get window surface
	surface = SDL_GetWindowSurface(window);

	//Fill the surface white
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));

	//Update the surface
	SDL_UpdateWindowSurface(window);

	SDL_FreeSurface(surface);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	// Create texture
	//texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	int channels = 4; // for a RGBA image
	int pitch = SCREEN_WIDTH * sizeof(unByte) * channels;
	unByte* pixels = new unByte[SCREEN_WIDTH * SCREEN_HEIGHT * channels];

	surface = SDL_GetWindowSurface(window);

	for (int r = 0; r < SCREEN_HEIGHT; r++) {
		for (int col = 0; col < SCREEN_WIDTH; col++) {
			unWord adr = (r / 8 * 0x100) + (r % 8) + (col / 8) * 0x10;

			unByte pixel = ((memory->PPURead(adr) >> (7 - (col % 8))) & 1) + ((memory->PPURead(adr + 8) >> (7 - (col % 8))) & 1) * 2;

			unByte RGB[] = { (Palette[pixel] >> 16) & 0xff, (Palette[pixel] >> 8) & 0xff , Palette[pixel] & 0xff};

			pixels[(r * SCREEN_WIDTH * channels) + (col * channels)] = 0x00;
			pixels[(r * SCREEN_WIDTH * channels) + (col * channels) + 1] = RGB[2];
			pixels[(r * SCREEN_WIDTH * channels) + (col * channels) + 2] = RGB[1];
			pixels[(r * SCREEN_WIDTH * channels) + (col * channels) + 3] = RGB[0];
		}
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_FreeSurface(surface);

	SDL_UpdateTexture(texture, NULL, pixels, pitch);

	while(true)
	{
		// Get the next event
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				// Break out of the loop on quit
				break;
			}
		}

		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_RenderPresent(renderer);
	}
	//Destroy texture
	SDL_DestroyTexture(texture);
	//Destroy renderer
	SDL_DestroyRenderer(renderer);
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();
}