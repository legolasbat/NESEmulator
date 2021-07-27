#include "NES.h"

#include <fstream>
#include <iostream>

NES::NES() {
	cpu.ConnectMemory(this);
}

void NES::Clock() {
	cpu.Clock();
}

void NES::PourMemory() {
	ppu.PrintTable();
}
