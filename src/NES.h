#pragma once

#include "Utils.h"
#include "CPU6502.h"
#include "PPU.h"
#include "Cartridge.h"

class NES
{
public:
	NES();
	void InsertCart(Cartridge* game);
	void Clock();

	void PourMemory();

	void CPUWrite(unWord dir, unByte b);
	unByte CPURead(unWord dir);
	void CPUWriteW(unWord dir, unWord w);
	unWord CPUReadW(unWord dir);

private:

	unByte memory[0x8000];

	Cartridge* cart;

	CPU cpu;
	PPU ppu;
};

