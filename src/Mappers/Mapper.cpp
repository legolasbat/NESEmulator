#include "Mapper.h"

Mapper::Mapper(unByte nPRGBanks, unByte nCHRBanks) {
	this->nPRGBanks = nPRGBanks;
	this->nCHRBanks = nCHRBanks;
}

void Mapper::Reset(){}

MIRROR Mapper::mirror() {
	return MIRROR::HARDWARE;
}

bool Mapper::IRQState() {
	return false;
}

void Mapper::IRQClear(){}

void Mapper::Scanline(){}
