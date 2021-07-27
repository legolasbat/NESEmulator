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
			unByte b = PPURead(i * 0x1000 + j);
			PatternTab[i][j] = b;
		}
	}
}

void PPU::Clock() {
	cycles++;

	if (cycles > 340) {
		cycles -= 341;
		scanLine++;
	}

	if (scanLine >= 0 && scanLine <= 239) {
		// Draw
	}
	else if (scanLine == 241 && cycles == 1) {
		// V Blank
		status.VerticalBlank = 1;
		NMI = true;
		// DrawNameTables
	}
	else if (scanLine == 261 && cycles == 1) {
		// V Blank off
		status.VerticalBlank = 0;
		NMI = false;
		scanLine = 0;
	}
}

void PPU::CPUWrite(unWord dir, unByte b) {
	switch (dir) {
	// Control
	case 0x0000:
		control.reg = b;
		// Refresh NameTables
		break;

	// Mask
	case 0x0001:
		mask.reg = b;
		break;

	// Status
	case 0x0002:
		break;

	// OAM Address
	case 0x0003:
		break;

	// OAM Data
	case 0x0004:
		break;

	// Scroll
	case 0x0005:
		break;

	// PPU Address
	case 0x0006:
		break;

	// PPU Data
	case 0x0007:
		break;
	}
}

unByte PPU::CPURead(unWord dir) {
	unByte val = 0;
	switch (dir) {
	// Control
	case 0x0000:
		// Not readable
		break;

	// Mask
	case 0x0001:
		// Not readable
		break;

		// Status
	case 0x0002:
		// Status stuff
		break;

		// OAM Address
	case 0x0003:
		// Not readable
		break;

		// OAM Data
	case 0x0004:
		// OAM Stuff
		break;

		// Scroll
	case 0x0005:
		// Not readable
		break;

		// PPU Address
	case 0x0006:
		// Not readable
		break;

		// PPU Data
	case 0x0007:
		// Too much stuff
		break;
	}
	return val;
}

void PPU::PPUWrite(unWord dir, unByte b) {

}

unByte PPU::PPURead(unWord dir) {
	unByte val = 0;
	dir &= 0x3fff;

	// Mapper...

	if (dir >= 0 && dir < 0x2000) {
		val = cart->PPURead(dir);
		//val = PatternTab[(dir & 0x1000) >> 12][dir & 0x0fff];
	}
	else {
		std::cout << "Not handle PPU read" << std::endl;
		return 0;
	}

	return val;
}

void PPU::GetPatternTab() {
	int channels = 4; // for a RGBA image
	int pitch = PATTERN_WIDTH * sizeof(unByte) * channels;
	unByte* pixels = new unByte[PATTERN_WIDTH * PATTERN_HEIGHT * channels];

	for (int r = 0; r < PATTERN_HEIGHT; r++) {
		for (int col = 0; col < PATTERN_WIDTH; col++) {
			unWord adr = (r / 8 * 0x100) + (r % 8) + (col / 8) * 0x10;

			unByte pixel = ((PatternTab[(adr & 0x1000) >> 12][adr & 0x0fff] >> (7 - (col % 8))) & 1) + ((PatternTab[((adr + 8) & 0x1000) >> 12][(adr + 8) & 0x0fff] >> (7 - (col % 8))) & 1) * 2;

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

		lo = PatternTab[(dir & 0x1000) >> 12][dir & 0x0fff];
		hi = PatternTab[((dir + 8) & 0x1000) >> 12][(dir + 8) & 0x0fff];

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