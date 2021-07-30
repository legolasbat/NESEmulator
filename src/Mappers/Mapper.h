#pragma once

#include "../Utils.h"

enum MIRROR
{
	HARDWARE,
	HORIZONTAL,
	VERTICAL,
	ONESCREEN_LO,
	ONESCREEN_HI,
};

class Mapper
{
public:
	Mapper(unByte nPRGBanks, unByte nCHRBanks);

	virtual bool CPUMapWrite(unWord dir, int& mappedDir, unByte data = 0) = 0;
	virtual bool CPUMapRead(unWord dir, int& mappedDir, unByte& data) = 0;

	virtual bool PPUMapWrite(unWord dir, int& mappedDir) = 0;
	virtual bool PPUMapRead(unWord dir, int& mappedDir) = 0;

	virtual void Reset() = 0;

	virtual MIRROR mirror();

	virtual bool IRQState();
	virtual void IRQClear();

	virtual void Scanline();

protected:

	unByte nPRGBanks = 0;
	unByte nCHRBanks = 0;
};

