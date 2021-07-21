#pragma once

#include "Memory.h"

#define SDL_MAIN_HANDLED
#include "SDL.h"

//Screen dimension constants
const int SCREEN_WIDTH = 128;	// 256				128 Palette Table
const int SCREEN_HEIGHT = 256;	// 224 NTSC 240 PAL	256 Palette Table
const int ZOOM = 3; // 3 Palette Table 50 Single Sprite

class PPU
{
public:
	void ConnectMemory(Memory *mem);
	void PrintTable();

private:
	Memory *memory;

	Uint32 Palettes[0x40] ={
							0x757575, 0x271b8f, 0x0000ab, 0x47009f, 0x8f0077, 0xab0013, 0xa70000, 0x7f0b00, 0x432f00, 0x004700, 0x005100, 0x003f17, 0x1b3f5f, 0x000000, 0x000000, 0x000000,
							0xbcbcbc, 0x0073ef, 0x233bef, 0x8300f3, 0xbf00bf, 0xe7005b, 0xdb2b00, 0xcb4f0f, 0x8b7300, 0x009700, 0x00ab00, 0x00933b, 0x00838b, 0x000000, 0x000000, 0x000000,
							0xffffff, 0x3fbfff, 0x5f97ff, 0xa78bfd, 0xf77bff, 0xff77b7, 0xff7763, 0xff9b3b, 0xf3bf3f, 0x83d313, 0x4fdf4b, 0x58f898, 0x00ebdb, 0x000000, 0x000000, 0x000000,
							0xffffff, 0xabe7ff, 0xc7d7ff, 0xd7cbff, 0xffc7ff, 0xffc7db, 0xffbfb3, 0xffdbab, 0xffe7a3, 0xe3ffa3, 0xabf3bf, 0xb3ffcf, 0x9ffff3, 0x000000, 0x000000, 0x000000
							};

	Uint32 Palette[4] = { Palettes[0x3f], Palettes[0x15], Palettes[0x2c], Palettes[0x07] };
};
