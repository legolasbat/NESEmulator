#include "NES.h"

#include <fstream>
#include <iostream>

void NES::InsertCart(Cartridge* game)
{
	cart = game;
	ppu.ConnectMemory(game);
}

void NES::CPUWrite(unWord dir, unByte b)
{
	if (cart->CPUWrite(dir, b)) {

	}
	else if (dir >= 0 && dir < 0x2000) {
		// RAM Range
		memory[dir & 0x07ff] = b;
	}
	else if(dir >= 0x2000 && dir < 0x4000){
		// PPU Range
		ppu.CPUWrite(dir & 0x0007, b);
	}
	else if((dir >= 0x4000 && dir <= 0x4013) || dir == 0x4015 || dir == 0x4017){
		// APU Range
		//std::cout << "Trying to write in APU" << std::endl;
	}
	else if (dir == 0x4014) {
		// DMA Transfer
		DMAPage = b;
		DMAAddr = 0;
		DMATransfer = true;
	}
	else if (dir >= 0x4016 && dir <= 0x4017) {
		// Controller Range
		controllerState[dir & 0x0001] = controller[dir & 0x0001];
	}
}

unByte NES::CPURead(unWord dir)
{
	unByte val = 0;

	if (cart->CPURead(dir, val)) {

	}
	else if (dir >= 0 && dir < 0x2000) {
		// RAM Range
		val = memory[dir & 0x07ff];
	}
	else if (dir >= 0x2000 && dir < 0x4000) {
		// PPU Range
		val = ppu.CPURead(dir & 0x0007);
	}
	else if (dir == 0x4015) {
		// APU Range
		//std::cout << "Trying to read in APU" << std::endl;
	}
	else if (dir >= 0x4016 && dir <= 0x4017) {
		// Controller Range
		val = (controllerState[dir & 0x0001] & 0x80) > 0;
		controllerState[dir & 0x0001] <<= 1;
	}

	return val;
}
