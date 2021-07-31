#include "Mapper0.h"

Mapper0::Mapper0(unByte nPRGBanks, unByte nCHRBanks) : Mapper(nPRGBanks, nCHRBanks)
{
}

bool Mapper0::CPUMapWrite(unWord dir, int& mappedDir, unByte data) {
	if (dir >= 0x8000) {
		mappedDir = dir & (nPRGBanks > 1 ? 0x7fff : 0x3fff);
		return true;
	}

	return false;
}

bool Mapper0::CPUMapRead(unWord dir, int& mappedDir, unByte& data) {
	if (dir >= 0x8000) {
		mappedDir = dir & (nPRGBanks > 1 ? 0x7fff : 0x3fff);
		return true;
	}

	return false;
}

bool Mapper0::PPUMapWrite(unWord dir, int& mappedDir) {
	if (dir < 0x2000) {
		if (nCHRBanks == 0) {
			mappedDir = dir;
			return true;
		}
	}

	return false;
}

bool Mapper0::PPUMapRead(unWord dir, int& mappedDir) {
	if (dir < 0x2000) {
		mappedDir = dir;
		return true;
	}

	return false;
}

void Mapper0::Reset() {};