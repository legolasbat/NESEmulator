#include "PPU.h"

#include <iostream>
#include <bitset>
#include <windows.h>

SDL_Texture* tabTexture;

PPU::PPU() :
	spriteMemory(64 * 4)
{
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

void PPU::Reset() {
	fineX = 0;
	addressLatch = 0;
	PPUDataBuffer = 0;
	scanLine = 0;
	cycles = 0;
	status.reg = 0;
	mask.reg = 0;
	control.reg = 0;
	vramAddr.reg = 0;
	tramAddr.reg = 0;
	oddFrame = false;

	pipelineState = PreRender;
	scanlineSprites.reserve(8);
	scanlineSprites.resize(0);
}

void PPU::Clock() {
	switch (pipelineState) {
	case PreRender:
		if (cycles == 1) {
			status.VerticalBlank = 0;
			spriteZeroHitPossible = false;
		}
		else if (cycles == ScanlineVisibleDots + 2 && mask.RenderBackground && mask.RenderSprites) {
			dataAddr &= ~0x41f;
			dataAddr |= tempAddr & 0x41f;
		}
		else if (cycles > 280 && cycles <= 304 && mask.RenderBackground && mask.RenderSprites) {
			dataAddr &= ~0x7be0;
			dataAddr |= tempAddr & 0x7be0;
		}

		if (cycles >= ScanlineEndCycle - (oddFrame && mask.RenderBackground && mask.RenderSprites)) {
			pipelineState = Render;
			cycles = scanLine = 0;
		}
		break;

	case Render:
		if (cycles > 0 && cycles <= ScanlineVisibleDots) {
			unByte BGColor = 0, FGColor = 0;
			bool BGOpaque = false, FGOpaque = true;
			bool SpriteFG = false;

			int x = cycles - 1;
			int y = scanLine;

			if (mask.RenderBackground) {

				unByte XFine = (fineX + x) % 8;

				if (mask.RenderBackgroundLeft || x >= 8) {
					unWord addr = 0x2000 | (dataAddr & 0x0fff);
					unByte tile = PPURead(addr);

					addr = (tile * 0x10) + ((dataAddr >> 12) & 0x7);
					addr |= control.PatternBackground << 12;

					BGColor = (PPURead(addr) >> (7 ^ XFine)) & 1;
					BGColor |= ((PPURead(addr + 8) >> (7 ^ XFine)) & 1) << 1;

					BGOpaque = BGColor;

					addr = 0x23c0 | (dataAddr & 0x0c00) | ((dataAddr >> 4) & 0x38)
						| ((dataAddr >> 2) & 0x07);

					unByte attr = PPURead(addr);
					int shift = ((dataAddr >> 4) & 4) | (dataAddr & 2);
					BGColor |= ((attr >> shift) & 0x3) << 2;
				}

				if (XFine == 7) {
					if ((dataAddr & 0x001f) == 31) {
						dataAddr &= ~0x001f;
						dataAddr ^= 0x0400;
					}
					else {
						dataAddr++;
					}
				}
			}

			if (mask.RenderSprites && (mask.RenderSpritesLeft || x >= 8)) {
				for (unByte i : scanlineSprites) {
					unByte SprX = spriteMemory[i * 4 + 3];

					if (0 > x - SprX || x - SprX >= 8)
						continue;

					unByte SprY = spriteMemory[i * 4] + 1,
						tile = spriteMemory[i * 4 + 1],
						attr = spriteMemory[i * 4 + 2];

					int length = (control.SpriteSize) ? 16 : 8;

					int XShift = (x - SprX) % 8, YOffset = (y - SprY) % length;

					if ((attr & 0x40) == 0)
						XShift ^= 7;
					if ((attr & 0x80) != 0)
						YOffset ^= (length - 1);

					unWord addr = 0;

					if (!control.SpriteSize) {
						addr = tile * 16 + YOffset;
						if (control.PatternSprite) addr += 0x1000;
					}
					else {
						YOffset = (YOffset & 7) | ((YOffset & 8) << 1);
						addr = (tile >> 1) * 32 + YOffset;
						addr |= (tile & 1) << 12;
					}

					FGColor |= (PPURead(addr) >> (XShift)) & 1;
					FGColor |= ((PPURead(addr + 8) >> (XShift)) & 1) << 1;

					if (!(FGOpaque = FGColor)) {
						FGColor = 0;
						continue;
					}

					FGColor |= 0x10;
					FGColor |= (attr & 0x3) << 2;

					SpriteFG = !(attr & 0x20);

					if (!spriteZeroHitPossible && mask.RenderBackground && i == 0 && FGOpaque && BGOpaque) {
						spriteZeroHitPossible = true;
					}

					break;
				}
			}

			unByte palAddr = BGColor;

			if ((!BGOpaque && FGOpaque) ||
				(BGOpaque && FGOpaque && SpriteFG)) {
				palAddr = FGColor;
			}
			else if (!BGOpaque && !FGOpaque)
				palAddr = 0;

			SetPixel(x, y, GetColorFromPalette(palAddr));
		}
		else if (cycles == ScanlineVisibleDots + 1 && mask.RenderBackground) {
			if ((dataAddr & 0x7000) != 0x7000)
				dataAddr += 0x1000;
			else {
				dataAddr &= ~0x7000;
				int y = (dataAddr & 0x03e0) >> 5;
				if (y == 29) {
					y = 0;
					dataAddr ^= 0x0800;
				}
				else if (y == 31)
					y = 0;
				else
					y++;
				dataAddr = (dataAddr & ~0x03e0) | (y << 5);
			}
		}
		else if (cycles == ScanlineVisibleDots + 2 && mask.RenderBackground && mask.RenderSprites) {
			dataAddr &= ~0x41f;
			dataAddr |= tempAddr & 0x41f;
		}

		if (cycles >= ScanlineEndCycle) {
			scanlineSprites.resize(0);

			int range = 8;
			if (control.SpriteSize)
				range = 16;

			std::size_t j = 0;
			for (std::size_t i = spriteDataAddr / 4; i < 64; i++) {
				int diff = (scanLine - spriteMemory[i * 4]);
				if (0 <= diff && diff < range) {
					scanlineSprites.push_back(i);
					j++;
					if (j >= 8) {
						break;
					}
				}
			}

			scanLine++;
			cycles = 0;
		}

		if (scanLine >= VisibleScanlines)
			pipelineState = PostRender;

		break;

	case PostRender:
		if (cycles >= ScanlineEndCycle) {
			scanLine++;
			cycles = 0;
			pipelineState = VerticalBlank;

			frameComplete = true;
		}
		break;

	case VerticalBlank:
		if (cycles == 1 && scanLine == VisibleScanlines + 1) {
			status.VerticalBlank = 1;
			if (control.EnableNMI)
				NMI = true;
		}

		if (cycles >= ScanlineEndCycle) {
			scanLine++;
			cycles = 0;
		}

		if (scanLine >= FrameEndScanline) {
			pipelineState = PreRender;
			scanLine = 0;
			oddFrame = !oddFrame;
		}
		break;
	}

	cycles++;

}

void PPU::SetPixel(int x, int y, Uint32 color) {

	unByte RGB[] = { (color >> 16) & 0xff, (color >> 8) & 0xff , color & 0xff };

	pixels[(y * SCREEN_WIDTH * channels) + (x * channels)] = 0;
	pixels[(y * SCREEN_WIDTH * channels) + (x * channels) + 1] = RGB[2];
	pixels[(y * SCREEN_WIDTH * channels) + (x * channels) + 2] = RGB[1];
	pixels[(y * SCREEN_WIDTH * channels) + (x * channels) + 3] = RGB[0];
}

Uint32 PPU::GetColorFromPalette(unByte palette) {
	return Palettes[PPURead(0x3f00 + palette) & 0x3f];
}

void PPU::DebugCycles() {
	std::cout << " PPU: " << scanLine << ", " << (cycles - 1) << std::endl;
}

void PPU::CPUWrite(unWord dir, unByte b) {
	switch (dir) {
	// Control
	case 0x0000:
		control.reg = b;
		tramAddr.nameTabX = control.NameTableX;
		tramAddr.nameTabY = control.NameTableY;
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
		OAMAddress = b;
		break;

	// OAM Data
	case 0x0004:
		pOAM[OAMAddress] = b;
		break;

	// Scroll
	case 0x0005:

		if (addressLatch == 0) {
			fineX = b & 0x07;
			tramAddr.coarseX = b >> 3;
			addressLatch = 1;
		}
		else {
			tramAddr.fineY = b & 0x07;
			tramAddr.coarseY = b >> 3;
			addressLatch = 0;
		}
		break;

	// PPU Address
	case 0x0006:
		if (addressLatch == 0) {
			tramAddr.reg = (unWord)((b & 0x3f) << 8) | (tramAddr.reg & 0x00ff);
			addressLatch = 1;
		}
		else {
			tramAddr.reg = (tramAddr.reg & 0xff00) | b;
			vramAddr = tramAddr;
			addressLatch = 0;
		}
		break;

	// PPU Data
	case 0x0007:
		PPUWrite(vramAddr.reg, b);

		vramAddr.reg += (control.IncMod ? 32 : 1);
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
		val = (status.reg & 0xe0) | (PPUDataBuffer & 0x1f);

		status.VerticalBlank = 0;

		addressLatch = 0;

		break;

		// OAM Address
	case 0x0003:
		// Not readable
		break;

		// OAM Data
	case 0x0004:
		val = pOAM[OAMAddress];
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
		
		val = PPUDataBuffer;
		PPUDataBuffer = PPURead(vramAddr.reg);

		if (vramAddr.reg >= 0x3f00) val = PPUDataBuffer;

		vramAddr.reg += (control.IncMod ? 32 : 1);

		break;
	}
	return val;
}

void PPU::PPUWrite(unWord dir, unByte b) {
	dir &= 0x3fff;

	if (cart->PPUWrite(dir, b)) {

	}
	else if (dir >= 0x0000 && dir < 0x2000) {
		PatternTab[(dir & 0x1000) >> 12][dir & 0x0fff] = b;
	}
	else if (dir >= 0x2000 && dir <= 0x3eff) {

		dir &= 0x0fff;
		MIRROR m = cart->Mirror();
		if (m == MIRROR::VERTICAL) {
			if (dir >= 0x0000 && dir <= 0x03FF)
				NameTab[0][dir & 0x03FF] = b;
			if (dir >= 0x0400 && dir <= 0x07FF)
				NameTab[1][dir & 0x03FF] = b;
			if (dir >= 0x0800 && dir <= 0x0BFF)
				NameTab[0][dir & 0x03FF] = b;
			if (dir >= 0x0C00 && dir <= 0x0FFF)
				NameTab[1][dir & 0x03FF] = b;
		}
		else if (m == MIRROR::HORIZONTAL) {
			if (dir >= 0x0000 && dir <= 0x03FF)
				NameTab[0][dir & 0x03FF] = b;
			if (dir >= 0x0400 && dir <= 0x07FF)
				NameTab[0][dir & 0x03FF] = b;
			if (dir >= 0x0800 && dir <= 0x0BFF)
				NameTab[1][dir & 0x03FF] = b;
			if (dir >= 0x0C00 && dir <= 0x0FFF)
				NameTab[1][dir & 0x03FF] = b;
		}
	}
	else if (dir >= 0x3f00 && dir <= 0x3fff) {

		dir &= 0x001f;
		if (dir == 0x0010) dir = 0x0000;
		if (dir == 0x0014) dir = 0x0004;
		if (dir == 0x0018) dir = 0x0008;
		if (dir == 0x001c) dir = 0x000c;
		PaletteTab[dir] = b;
	}
}

unByte PPU::PPURead(unWord dir) {
	unByte val = 0;
	dir &= 0x3fff;

	if (cart->PPURead(dir, val)) {

	}
	else if (dir >= 0 && dir < 0x2000) {
		val = PatternTab[(dir & 0x1000) >> 12][dir & 0x0fff];
	}
	else if (dir >= 0x2000 && dir <= 0x3eff) {

		dir &= 0x0fff;
		MIRROR m = cart->Mirror();
		if (m == MIRROR::VERTICAL) {
			if (dir >= 0x0000 && dir <= 0x03FF)
				val = NameTab[0][dir & 0x03FF];
			if (dir >= 0x0400 && dir <= 0x07FF)
				val = NameTab[1][dir & 0x03FF];
			if (dir >= 0x0800 && dir <= 0x0BFF)
				val = NameTab[0][dir & 0x03FF];
			if (dir >= 0x0C00 && dir <= 0x0FFF)
				val = NameTab[1][dir & 0x03FF];
		}
		else if (m == MIRROR::HORIZONTAL) {
			if (dir >= 0x0000 && dir <= 0x03FF)
				val = NameTab[0][dir & 0x03FF];
			if (dir >= 0x0400 && dir <= 0x07FF)
				val = NameTab[0][dir & 0x03FF];
			if (dir >= 0x0800 && dir <= 0x0BFF)
				val = NameTab[1][dir & 0x03FF];
			if (dir >= 0x0C00 && dir <= 0x0FFF)
				val = NameTab[1][dir & 0x03FF];
		}
	}
	else if (dir >= 0x3f00 && dir <= 0x3fff) {

		dir &= 0x001f;
		if (dir == 0x0010) dir = 0x0000;
		if (dir == 0x0014) dir = 0x0004;
		if (dir == 0x0018) dir = 0x0008;
		if (dir == 0x001c) dir = 0x000c;
		val = PaletteTab[dir] & (mask.Grayscale ? 0x30 : 0x3f);
	}

	return val;
}

void PPU::GetPatternTab() {
	int TabPitch = PATTERN_WIDTH * sizeof(unByte) * channels;
	unByte* TabPixels = new unByte[PATTERN_WIDTH * PATTERN_HEIGHT * channels];

	for (int r = 0; r < PATTERN_HEIGHT; r++) {
		for (int col = 0; col < PATTERN_WIDTH; col++) {
			unWord adr = (r / 8 * 0x100) + (r % 8) + (col / 8) * 0x10;

			unByte pixel = ((PatternTab[(adr & 0x1000) >> 12][adr & 0x0fff] >> (7 - (col % 8))) & 1) + ((PatternTab[((adr + 8) & 0x1000) >> 12][(adr + 8) & 0x0fff] >> (7 - (col % 8))) & 1) * 2;

			unByte RGB[] = { (Palette[pixel] >> 16) & 0xff, (Palette[pixel] >> 8) & 0xff , Palette[pixel] & 0xff };

			TabPixels[(r * PATTERN_WIDTH * channels) + (col * channels)] = 0x00;
			TabPixels[(r * PATTERN_WIDTH * channels) + (col * channels) + 1] = RGB[2];
			TabPixels[(r * PATTERN_WIDTH * channels) + (col * channels) + 2] = RGB[1];
			TabPixels[(r * PATTERN_WIDTH * channels) + (col * channels) + 3] = RGB[0];
		}
	}

	SDL_UpdateTexture(tabTexture, NULL, TabPixels, TabPitch);
}

void PPU::GetNameTab() {
	int TabPitch = SCREEN_WIDTH * sizeof(unByte) * channels;
	unByte* TabPixels = new unByte[SCREEN_WIDTH * 1024 * channels];

	for (int r = 0; r < 1024; r++) {
		for (int col = 0; col < SCREEN_WIDTH; col++) {
			unWord tile_nr = PPURead(0x2000 + (r / 8 * 32) + (col / 8));

			unWord adr = (control.PatternBackground ? 0x1000 : 0) + (r % 8) + tile_nr * 0x10;

			unByte pixel = ((PPURead(adr) >> (7 - (col % 8))) & 1) + (((PPURead(adr + 8) >> (7 - (col % 8))) & 1) * 2);
			
			unByte RGB[] = { (Palette[pixel] >> 16) & 0xff, (Palette[pixel] >> 8) & 0xff , Palette[pixel] & 0xff };

			TabPixels[(r * SCREEN_WIDTH * channels) + (col * channels)] = 0x00;
			TabPixels[(r * SCREEN_WIDTH * channels) + (col * channels) + 1] = RGB[2];
			TabPixels[(r * SCREEN_WIDTH * channels) + (col * channels) + 2] = RGB[1];
			TabPixels[(r * SCREEN_WIDTH * channels) + (col * channels) + 3] = RGB[0];
		}
	}

	SDL_UpdateTexture(tabTexture, NULL, TabPixels, TabPitch);
}

void PPU::PrintTable(int t) {

	int width;
	int height;
	int zoom = ZOOM;

	if (t == 0) {
		width = SCREEN_WIDTH;
		height = 1024;
	}
	else if (t == 1) {
		width = PATTERN_WIDTH;
		height = PATTERN_HEIGHT;
	}
	else {
		width = 8;
		height = 8;
		zoom = 50;
	}

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	//Create window
	SDL_Window* tabWindow = SDL_CreateWindow("Sprites Tables", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (tabWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	SDL_SetWindowSize(tabWindow, width * zoom, height * zoom);
	SDL_SetWindowResizable(tabWindow, SDL_TRUE);

	//Get window surface
	SDL_Surface* tabSurface = SDL_GetWindowSurface(tabWindow);

	//Fill the surface white
	SDL_FillRect(tabSurface, NULL, SDL_MapRGB(tabSurface->format, 0x00, 0x00, 0x00));

	//Update the surface
	SDL_UpdateWindowSurface(tabWindow);

	SDL_FreeSurface(tabSurface);

	SDL_Renderer* tabRenderer = SDL_CreateRenderer(tabWindow, -1, SDL_RENDERER_ACCELERATED);

	// Create texture
	tabTexture = SDL_CreateTexture(tabRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

	// Get texture
	if (t == 0) {
		GetNameTab();
	}
	else if (t == 1) {
		GetPatternTab();
	}

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

		SDL_RenderClear(tabRenderer);

		SDL_RenderCopy(tabRenderer, tabTexture, NULL, NULL);

		SDL_RenderPresent(tabRenderer);
	}
	//Destroy texture
	SDL_DestroyTexture(tabTexture);
	//Destroy renderer
	SDL_DestroyRenderer(tabRenderer);
	//Destroy window
	SDL_DestroyWindow(tabWindow);
}