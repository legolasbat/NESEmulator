#pragma once

#include "Utils.h"
#include "Cartridge.h"

#include <vector>

class Memory {
private:
	unByte memory[0x8000];

	Cartridge *cart;

public:
	void Load(const char* game);
	void CPUWrite(unWord dir, unByte b);
	unByte CPURead(unWord dir);
	void CPUWriteW(unWord dir, unWord w);
	unWord CPUReadW(unWord dir);

	unByte PPURead(unWord dir);

	void PrintMemory();
};