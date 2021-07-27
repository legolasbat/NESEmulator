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
	//std::cout << "Writing Byte in Address: " << std::hex << dir << " Value: " << std::hex << (int)b << std::endl;
	if (dir >= 0 && dir < 0x8000) {
		if (dir >= 0 && dir < 0x2000) {
			memory[dir & 0x07ff] = b;
		}
		else if(dir >= 0x2000 && dir < 0x4000){
			memory[dir & 0x0007] = b;
		}
		else {
			memory[dir] = b;
		}
	}
	else
		std::cout << "Trying to write in ROM" << std::endl;
}

unByte NES::CPURead(unWord dir)
{
	unByte val;
	bool isCart = false;
	if (dir >= 0 && dir < 0x8000)
		val = memory[dir];
	else {
		val = cart->CPURead(dir);
		isCart = true;
	}

	//std::cout << "Reading Byte in " << (cart ? "Address in file: " : "Address: ") << std::hex << (isCart ? (dir & 0x7fff) : dir) << " Value: " << std::hex << (int)val << std::endl;
	return val;
}

void NES::CPUWriteW(unWord dir, unWord w)
{
	//std::cout << "Writing Word in Address: " << std::hex << dir << " Value: " << std::hex << (int)w << std::endl;
	if (dir >= 0 && dir < 0x7fff) {
		unByte v = w & 0x00ff;
		memory[dir] = v;
		v = (w & 0xff00) >> 8;
		memory[dir + 1] = v;
	}
	else {
		std::cout << "Trying to write in ROM" << std::endl;
	}
	
}

unWord NES::CPUReadW(unWord dir)
{
	unWord val;
	bool isCart = false;
	if (dir >= 0 && dir < 0x8000) {
		val = memory[dir];
		val = val | (memory[dir + 1] << 8);
	}
	else {
		//unWord tempDir = dir & ((nPRGBanks > 1) ? 0x7fff : 0x3fff);
		val = cart->CPURead(dir);
		val = val | (cart->CPURead(dir + 1) << 8);
		isCart = true;
	}

	//std::cout << "Reading Word in " << (cart ? "Address in file: " : "Address: ") << std::hex << (isCart ? (dir & 0x7fff) : dir) << " Value: " << std::hex << (int)val << std::endl;
	return val;
}
