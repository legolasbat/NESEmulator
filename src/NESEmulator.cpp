#include "NES.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include "SDL.h"

//#define PI2 6.28318530718
//
//float fTime = 0;
//
//static void callback(void* userdate, Uint8* stream, int len) {
//	Sint16* stream16 = (Sint16*)stream;
//
//	int nb_samples = len / sizeof(Sint16);
//	for (int i = 0; i < nb_samples; i++) {
//		stream16[i] = 32000 * sin(fTime);
//
//		fTime += 440.0 * PI2 / 44100.0;
//		if (fTime >= PI2)
//			fTime -= PI2;
//	}
//}

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


	//// Audio
	//SDL_Init(SDL_INIT_AUDIO);
	//
	//SDL_AudioDeviceID audio_device;
	//
	//SDL_AudioSpec audio_spec = { 0 };
	//audio_spec.freq = 44100;
	//audio_spec.format = AUDIO_S16SYS;
	//audio_spec.channels = 1;
	//audio_spec.samples = 4096;
	//audio_spec.callback = callback;
	//audio_spec.userdata = NULL;
	//
	//audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
	//
	//SDL_PauseAudioDevice(audio_device, 0);
	//// End audio


	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	// Create texture
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	Cartridge* cart = new Cartridge("nestest.nes");
	//Cartridge* cart = new Cartridge("Donkey Kong.nes");
	//Cartridge* cart = new Cartridge("Excitebike.nes");

	NES nes;

	nes.InsertCart(cart);

	nes.Reset();

	SDL_RenderClear(renderer);

	SDL_UpdateTexture(texture, NULL, nes.ppu.pixels, nes.ppu.pitch);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);

	SDL_Event event;
	bool play = true;

	std::chrono::high_resolution_clock::time_point end;
	std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();

	while (play)
	{
		nes.Clock();

		if (nes.ppu.frameComplete) {
			// Get the next event
			while (SDL_PollEvent(&event))
			{
				switch (event.type) {
				case SDL_QUIT:
					// Break out of the loop on quit
					play = false;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						// A
					case SDLK_m: nes.controller[0] |= 0x80; break;
						// B
					case SDLK_n: nes.controller[0] |= 0x40; break;
						// Select
					case SDLK_LCTRL: nes.controller[0] |= 0x20; break;
						// Start
					case SDLK_RETURN: nes.controller[0] |= 0x10; break;
						// Up
					case SDLK_w: nes.controller[0] |= 0x08; break;
						// Down
					case SDLK_s: nes.controller[0] |= 0x04; break;
						// Left
					case SDLK_a: nes.controller[0] |= 0x02; break;
						// Right
					case SDLK_d: nes.controller[0] |= 0x01; break;
						// Escape
					case SDLK_ESCAPE: play = false; break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
						// A
					case SDLK_m: nes.controller[0] &= ~0x80; break;
						// B
					case SDLK_n: nes.controller[0] &= ~0x40; break;
						// Select
					case SDLK_LCTRL: nes.controller[0] &= ~0x20; break;
						// Start
					case SDLK_RETURN: nes.controller[0] &= ~0x10; break;
						// Up
					case SDLK_w: nes.controller[0] &= ~0x08; break;
						// Down
					case SDLK_s: nes.controller[0] &= ~0x04; break;
						// Left
					case SDLK_a: nes.controller[0] &= ~0x02; break;
						// Right
					case SDLK_d: nes.controller[0] &= ~0x01; break;
					}
					break;
				}
			}
			SDL_RenderClear(renderer);
			SDL_UpdateTexture(texture, NULL, nes.ppu.pixels, nes.ppu.pitch);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			nes.ppu.frameComplete = false;

			end = std::chrono::high_resolution_clock::now();
			std::cout << "Time frame = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
			begin = std::chrono::high_resolution_clock::now();
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