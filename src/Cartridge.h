#pragma once

#include "Utils.h"

#include <vector>

class Cartridge
{
public:
	Cartridge(const char* game);

	//void Write(unWord dir, unByte b);
	unByte CPURead(unWord dir);
	//void WriteW(unWord dir, unWord w);
	//unWord ReadW(unWord dir);

	unByte PPURead(unWord dir);

private:

	std::vector<unByte> PRGMemory;
	std::vector<unByte> CHRMemory;

	unByte mapperID = 0;
	unByte nPRGBanks = 0;
	unByte nCHRBanks = 0;
};

