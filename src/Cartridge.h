#pragma once

#include "Utils.h"
#include "Mappers/Mapper0.h"

#include <fstream>
#include <vector>

class Cartridge
{
public:
	Cartridge(const char* game);



	bool CPUWrite(unWord dir, unByte b);
	bool CPURead(unWord dir, unByte &b);

	bool PPUWrite(unWord dir, unByte b);
	bool PPURead(unWord dir, unByte &b);

	void Reset();

	MIRROR Mirror();

	std::shared_ptr<Mapper> GetMapper();

private:

	std::vector<unByte> PRGMemory;
	std::vector<unByte> CHRMemory;

	unByte mapperID = 0;
	unByte nPRGBanks = 0;
	unByte nCHRBanks = 0;

	MIRROR HWMirror = HORIZONTAL;

	std::shared_ptr<Mapper> mapper;
};

