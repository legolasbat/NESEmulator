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

private:

	int clockCounter = 0;

	unByte memory[0x800];

	unByte controller[2];

	Cartridge* cart = nullptr;

	CPU cpu;
	PPU ppu;
};

