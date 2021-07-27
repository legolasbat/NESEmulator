#include "NES.h"

#include <fstream>
#include <iostream>

NES::NES() {
	cpu.ConnectMemory(this);
}

void NES::Clock() {

	ppu.Clock();

	// APU Clock

	if (clockCounter % 3 == 0) {
		// Check and perform a DMA

		// else
		cpu.Clock();
	}

	// Audio synch

	if (ppu.NMI) {
		ppu.NMI = false;
		cpu.NMI();
	}

	// Check mapper for IRQ

	clockCounter++;
}

void NES::PourMemory() {
	ppu.PrintTable();
}
