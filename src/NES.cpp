#include "NES.h"

#include <fstream>
#include <iostream>

NES::NES() {
	cpu = new CPU();
	cpu->ConnectMemory(&cpuMem);
	ppu = new PPU();
	ppu->ConnectMemory(&cpuMem);
}

void NES::Load(const char* game) {
	cpuMem.Load(game);
}

void NES::Clock() {
	cpu->Clock();
}

void NES::PourMemory() {
	//cpuMem.PrintMemory();
	ppu->PrintTable();
}
