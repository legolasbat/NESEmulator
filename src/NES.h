#pragma once

#include "Utils.h"
#include "CPU6502.h"
#include "PPU.h"
#include "Memory.h"
#include "Cartridge.h"

class NES
{
public:
	NES();
	void Load(const char* game);
	void Clock();

	void PourMemory();

private:
	CPU *cpu;
	PPU *ppu;

	Memory cpuMem;
};

