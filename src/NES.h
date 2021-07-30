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
	void Reset();
	void Clock();

	void CPUWrite(unWord dir, unByte b);
	unByte CPURead(unWord dir);

	void DebugPPU();

	CPU cpu;
	PPU ppu;

	unByte controller[2];

	int clockCounter = 0;

private:

	unByte memory[0x800];

	unByte controllerState[2];

	Cartridge* cart = nullptr;

	unByte DMAPage = 0;
	unByte DMAAddr = 0;
	unByte DMAData = 0;

	bool DMA = true;

	bool DMATransfer = false;
};

