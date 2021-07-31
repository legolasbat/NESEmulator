#pragma once

#include "Cartridge.h"

#define SDL_MAIN_HANDLED
#include "SDL.h"

//Screen dimension constants
const int SCREEN_WIDTH = 256;	// 256
const int SCREEN_HEIGHT = 240;	// 224 NTSC 240 PAL
const int ZOOM = 3; // 3 Palette Table 50 Single Sprite

const int PATTERN_WIDTH = 128; // 256
const int PATTERN_HEIGHT = 256;

const int ScanlineCycleLength = 341;
const int ScanlineEndCycle = 340;
const int VisibleScanlines = 240;
const int ScanlineVisibleDots = 256;
const int FrameEndScanline = 261;

class PPU
{
public:
	PPU();
	~PPU();

	void ConnectMemory(Cartridge *mem);

	void Reset();

	void Clock();

	void SetPixel(int x, int y, Uint32 color);
	Uint32 GetColorFromPalette(unByte palette);

	void CPUWrite(unWord dir, unByte b);
	unByte CPURead(unWord dir);

	void PPUWrite(unWord dir, unByte b);
	unByte PPURead(unWord dir);

	void GetPatternTab();
	void GetNameTab();
	void PrintTable(int t);

	void DebugCycles();

	bool NMI = false;

	unByte* pOAM = (unByte*)OAM;

	bool frameComplete = false;

private:

	enum State {
		PreRender,
		Render,
		PostRender,
		VerticalBlank
	} pipelineState;

	unWord dataAddr = 0;
	unWord tempAddr = 0;

	int cycles = 0;
	int scanLine = 0;
	bool oddFrame = false;

	Cartridge *cart = nullptr;

	unByte PaletteTab[0x20];
	unByte NameTab[2][0x400];	// Two tables of 0x400 bytes
	unByte PatternTab[2][4096];	// Two tables of 0x1000 bytes

	Uint32 Palettes[0x40] ={
							0x757575, 0x271b8f, 0x0000ab, 0x47009f, 0x8f0077, 0xab0013, 0xa70000, 0x7f0b00, 0x432f00, 0x004700, 0x005100, 0x003f17, 0x1b3f5f, 0x000000, 0x000000, 0x000000,
							0xbcbcbc, 0x0073ef, 0x233bef, 0x8300f3, 0xbf00bf, 0xe7005b, 0xdb2b00, 0xcb4f0f, 0x8b7300, 0x009700, 0x00ab00, 0x00933b, 0x00838b, 0x000000, 0x000000, 0x000000,
							0xffffff, 0x3fbfff, 0x5f97ff, 0xa78bfd, 0xf77bff, 0xff77b7, 0xff7763, 0xff9b3b, 0xf3bf3f, 0x83d313, 0x4fdf4b, 0x58f898, 0x00ebdb, 0x000000, 0x000000, 0x000000,
							0xffffff, 0xabe7ff, 0xc7d7ff, 0xd7cbff, 0xffc7ff, 0xffc7db, 0xffbfb3, 0xffdbab, 0xffe7a3, 0xe3ffa3, 0xabf3bf, 0xb3ffcf, 0x9ffff3, 0x000000, 0x000000, 0x000000
							};

	Uint32 Palette[4] = { Palettes[0x3f], Palettes[0x15], Palettes[0x2c], Palettes[0x07] };

	// 0x2000
	union PPUCTRL
	{
		struct
		{
			unByte NameTableX : 1;
			unByte NameTableY : 1;
			unByte IncMod : 1;
			unByte PatternSprite : 1;
			unByte PatternBackground : 1;
			unByte SpriteSize : 1;
			unByte SlaveMode : 1; // Unused
			unByte EnableNMI : 1;
		};

		unByte reg;
	} control;

	// 0x2001
	union PPUMASK
	{
		struct
		{
			uint8_t Grayscale : 1;
			uint8_t RenderBackgroundLeft : 1;
			uint8_t RenderSpritesLeft : 1;
			uint8_t RenderBackground : 1;
			uint8_t RenderSprites : 1;
			uint8_t EmphasizeRed : 1;
			uint8_t EmphasizeGreen : 1;
			uint8_t EmphasizeBlue : 1;
		};

		uint8_t reg;
	} mask;

	// 0x2002
	union PPUSTATUS
	{
		struct
		{
			uint8_t Unused : 5;
			uint8_t SpriteOverflow : 1;
			uint8_t SpriteZeroHit : 1;
			uint8_t VerticalBlank : 1;
		};

		uint8_t reg;
	} status;

	union REGISTER {
		struct {
			unWord coarseX : 5;
			unWord coarseY : 5;
			unWord nameTabX : 1;
			unWord nameTabY : 1;
			unWord fineY : 3;
			unWord unused : 1;
		};

		unWord reg = 0;
	};

	REGISTER vramAddr;
	REGISTER tramAddr;

	unByte fineX = 0;

	unByte addressLatch;
	unByte PPUDataBuffer = 0;

	struct ObjectAttrEntry {
		unByte y;		// Y pos of sprite
		unByte id;		// ID of tile
		unByte attr;	// Flags for sprite
		unByte x;		// X pos of sprite
	} OAM[64];

	unByte OAMAddress = 0;

public:
	std::vector<unByte> spriteMemory;

private:
	std::vector<unByte> scanlineSprites;
	unByte spriteDataAddr = 0;

	unByte spriteCount;

	bool spriteZeroHitPossible = false;
	bool spriteZeroBeingRendered = false;

public:
	int channels = 4; // for a RGBA image
	int pitch = SCREEN_WIDTH * sizeof(unByte) * channels;
	unByte* pixels = new unByte[SCREEN_WIDTH * SCREEN_HEIGHT * channels];
};

