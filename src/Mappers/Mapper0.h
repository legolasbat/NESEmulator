#pragma once

#include "Mapper.h"

class Mapper0 : public Mapper
{
public:
	Mapper0(unByte nPRGBanks, unByte nCHRBanks);

	bool CPUMapWrite(unWord dir, int& mappedDir, unByte data = 0) override;
	bool CPUMapRead(unWord dir, int& mappedDir, unByte& data) override;

	bool PPUMapWrite(unWord dir, int& mappedDir) override;
	bool PPUMapRead(unWord dir, int& mappedDir) override;
	void Reset() override;
};

