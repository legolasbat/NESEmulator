#include "NES.h"

#include <fstream>
#include <iostream>

NES::NES() {
	cpu.ConnectMemory(this);
}

void NES::Reset() {
	cart->Reset();
	cpu.Reset();
	ppu.Reset();
	clockCounter = 0;
	DMAPage = 0;
	DMAAddr = 0;
	DMAData = 0;
	DMA = true;
	DMATransfer = false;
}

void NES::Clock() {
	ppu.Clock();

	// APU Clock

	if (clockCounter % 3 == 0) {
		if (DMATransfer) {
			if (DMA) {
				if (clockCounter % 2 == 1) {
					DMA = false;
				}
			}
			else {
				if (clockCounter % 2 == 0) {
					DMAData = CPURead(DMAPage << 8 | DMAAddr);
				}
				else {
					//ppu.pOAM[DMAAddr] = DMAData;
					ppu.spriteMemory[DMAAddr] = DMAData;
					DMAAddr++;
					if (DMAAddr == 0x00) {
						DMATransfer = false;
						DMA = true;
					}
				}
			}
		}
		else {
			cpu.Clock();
		}
	}

	// Audio synch

	if (ppu.NMI) {
		ppu.NMI = false;
		cpu.NMI();
	}

	// Check mapper for IRQ

	clockCounter++;
}

void NES::DebugPPU() {
	ppu.DebugCycles();
}
