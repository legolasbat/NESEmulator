#include "PPU.h"

#include <iostream>
#include <bitset>
#include <windows.h>

SDL_Window* window;

SDL_Surface* surface;

SDL_Renderer* renderer;
SDL_Texture* texture;

PPU::PPU() {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
}

PPU::~PPU() {
	//Quit SDL subsystems
	SDL_Quit();
}

void PPU::ConnectMemory(Cartridge *mem) {
	cart = mem;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4096; j++) {
			PatternTab[i][j] = PPURead(i * 0x1000 + j);
		}
	}
}

void PPU::PPUWrite(unWord dir, unByte b) {

}

unByte PPU::PPURead(unWord dir) {
	if (dir >= 0 && dir < 0x2000) {
		return cart->PPURead(dir);
	}
	else {
		std::cout << "Not handle PPU read" << std::endl;
		return 0;
	}
}

void PPU::GetPatternTab() {
	int channels = 4; // for a RGBA image
	int pitch = PATTERN_WIDTH * sizeof(unByte) * channels;
	unByte* pixels = new unByte[PATTERN_WIDTH * PATTERN_HEIGHT * channels];

	for (int r = 0; r < PATTERN_HEIGHT; r++) {
		for (int col = 0; col < PATTERN_WIDTH; col++) {
			unWord adr = (r / 8 * 0x100) + (r % 8) + (col / 8) * 0x10;

			unByte pixel = ((PPURead(adr) >> (7 - (col % 8))) & 1) + ((PPURead(adr + 8) >> (7 - (col % 8))) & 1) * 2;

			unByte RGB[] = { (Palette[pixel] >> 16) & 0xff, (Palette[pixel] >> 8) & 0xff , Palette[pixel] & 0xff };

			pixels[(r * PATTERN_WIDTH * channels) + (col * channels)] = 0x00;
			pixels[(r * PATTERN_WIDTH * channels) + (col * channels) + 1] = RGB[2];
			pixels[(r * PATTERN_WIDTH * channels) + (col * channels) + 2] = RGB[1];
			pixels[(r * PATTERN_WIDTH * channels) + (col * channels) + 3] = RGB[0];
		}
	}

	// Create texture
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PATTERN_WIDTH, PATTERN_HEIGHT);

	SDL_UpdateTexture(texture, NULL, pixels, pitch);
}

void PPU::GetSprite(int i) {
	int channels = 4; // for a RGBA image
	int pitch = PATTERN_WIDTH * sizeof(unByte) * channels;
	unByte* pixels = new unByte[PATTERN_WIDTH * PATTERN_HEIGHT * channels];

	unWord dir = (i * 0x10);
	unByte lo;
	unByte hi;

	for (int r = 0; r < 8; r++) {

		lo = PPURead(dir);
		hi = PPURead(dir + 8);

		std::bitset<8> x(lo);
		std::bitset<8> y(hi);
		std::cout << "Low: " << x << std::endl;
		std::cout << "High: " << y << std::endl;

		for (int c = 0; c < 8; c++) {

			unByte pixel = ((lo >> (7 - (c % 8))) & 1) + (((hi >> (7 - (c % 8))) & 1) * 2);

			std::cout << "Pixel: " << +pixel << " ";

			unByte RGB[] = { (Palette[pixel] >> 16) & 0xff, (Palette[pixel] >> 8) & 0xff , Palette[pixel] & 0xff };

			pixels[(r * PATTERN_WIDTH * channels) + (c * channels)] = 0x00;
			pixels[(r * PATTERN_WIDTH * channels) + (c * channels) + 1] = RGB[2];
			pixels[(r * PATTERN_WIDTH * channels) + (c * channels) + 2] = RGB[1];
			pixels[(r * PATTERN_WIDTH * channels) + (c * channels) + 3] = RGB[0];
		}
		std::cout << std::endl;
		std::cout << std::endl;
		dir++;
	}

	// Create texture
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PATTERN_WIDTH, PATTERN_HEIGHT);

	SDL_UpdateTexture(texture, NULL, pixels, pitch);
}

void PPU::PrintTable() {

	//Create window
	window = SDL_CreateWindow("NESEmulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, PATTERN_WIDTH, PATTERN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	SDL_SetWindowSize(window, PATTERN_WIDTH* ZOOM, PATTERN_HEIGHT * ZOOM);
	SDL_SetWindowResizable(window, SDL_TRUE);

	//Get window surface
	surface = SDL_GetWindowSurface(window);

	//Fill the surface white
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));

	//Update the surface
	SDL_UpdateWindowSurface(window);

	SDL_FreeSurface(surface);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Get texture
	GetPatternTab();

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
}