#include "NES.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include "SDL.h"

int main(void) {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	//Create window
	SDL_Window* window = SDL_CreateWindow("NESEmulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	SDL_SetWindowSize(window, SCREEN_WIDTH * ZOOM, SCREEN_HEIGHT * ZOOM);
	SDL_SetWindowResizable(window, SDL_TRUE);

	//Get window surface
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	//Fill the surface white
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));

	//Update the surface
	SDL_UpdateWindowSurface(window);

	SDL_FreeSurface(surface);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Create texture
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Cartridge* cart = new Cartridge("nestest.nes");
	Cartridge* cart = new Cartridge("Donkey Kong.nes");

	NES nes;

	nes.InsertCart(cart);

	nes.Reset();

	SDL_RenderClear(renderer);

	SDL_UpdateTexture(texture, NULL, nes.ppu.pixels, nes.ppu.pitch);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);

	bool play = true;
	while (play)
	{
		// Get the next event
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type) {
			case SDL_QUIT:
				// Break out of the loop on quit
				play = false;
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				SDL_KeyboardEvent key = event.key;
				if (key.type == SDL_KEYDOWN) {
					std::string k = SDL_GetKeyName(key.keysym.sym);
					//std::cout << SDL_GetKeyName(key.keysym.sym) << std::endl;
					// A
					if (k == "M") {
						nes.controller[0] |= 0x80;
					}
					// B
					else if (k == "N") {
						nes.controller[0] |= 0x40;
					}
					// Select
					else if (k == "Left Ctrl") {
						nes.controller[0] |= 0x20;
					}
					// Start
					else if (k == "Return") {
						nes.controller[0] |= 0x10;
					}
					// Up
					else if (k == "W") {
						nes.controller[0] |= 0x08;
					}
					// Down
					else if (k == "S") {
						nes.controller[0] |= 0x04;
					}
					// Left
					else if (k == "A") {
						nes.controller[0] |= 0x02;
					}
					// Right
					else if (k == "D") {
						nes.controller[0] |= 1;
					}
					else if (k == "Escape") {
						play = false;
					}
				}
				else if (key.type == SDL_KEYUP) {
					std::string k = SDL_GetKeyName(key.keysym.sym);
					// A
					if (k == "M") {
						nes.controller[0] &= ~0x80;
					}
					// B
					else if (k == "N") {
						nes.controller[0] &= ~0x40;
					}
					// Select
					else if (k == "Left Ctrl") {
						nes.controller[0] &= ~0x20;
					}
					// Start
					else if (k == "Return") {
						nes.controller[0] &= ~0x10;
					}
					// Up
					else if (k == "W") {
						nes.controller[0] &= ~0x08;
					}
					// Down
					else if (k == "S") {
						nes.controller[0] &= ~0x04;
					}
					// Left
					else if (k == "A") {
						nes.controller[0] &= ~0x02;
					}
					// Right
					else if (k == "D") {
						nes.controller[0] &= ~1;
					}
				}
				break;
			}
		}

		nes.Clock();

		if (nes.ppu.frameComplete) {
			SDL_RenderClear(renderer);
			SDL_UpdateTexture(texture, NULL, nes.ppu.pixels, nes.ppu.pitch);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			nes.ppu.frameComplete = false;
		}
	}

	//Destroy texture
	SDL_DestroyTexture(texture);
	//Destroy renderer
	SDL_DestroyRenderer(renderer);
	//Destroy window
	SDL_DestroyWindow(window);

	SDL_Quit();
	

	//while (!nes.Clock()) {}

	//nes.ppu.PrintTable(1);

	return 0;
}