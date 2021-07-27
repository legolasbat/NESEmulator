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
	if (dir >= 0 && dir < 0x8000) {
		if (dir >= 0 && dir < 0x2000) {
			// RAM Range
			memory[dir & 0x07ff] = b;
		}
		else if(dir >= 0x2000 && dir < 0x4000){
			// PPU Range
			ppu.CPUWrite(dir, b);
		}
		else if((dir >= 0x4000 && dir <= 0x4013) || dir == 0x4015 || dir == 0x4017){
			// APU Range
			std::cout << "Trying to write in APU" << std::endl;
		}
		else if (dir == 0x4014) {
			// DMA Transfer
			std::cout << "DMA Transfer" << std::endl;
		}
		else if (dir >= 0x4016 && dir <= 0x4017) {
			// Controller Range
			std::cout << "Writing in controller?" << std::endl;
		}
	}
	else
		std::cout << "Trying to write in ROM" << std::endl;
}

unByte NES::CPURead(unWord dir)
{
	unByte val = 0;
	if (dir >= 0 && dir < 0x8000)
		if (dir >= 0 && dir < 0x2000) {
			// RAM Range
			val = memory[dir & 0x07ff];
		}
		else if (dir >= 0x2000 && dir < 0x4000) {
			// PPU Range
			val = ppu.CPURead(dir & 0x0007);
		}
		else if (dir == 0x4015) {
			// APU Range
			std::cout << "Trying to read in APU" << std::endl;
		}
		else if (dir >= 0x4016 && dir <= 0x4017) {
			// Controller Range
			std::cout << "Reading from controller" << std::endl;
		}
	else {
		val = cart->CPURead(dir);
	}

	return val;
}
