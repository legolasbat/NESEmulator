#include "Cartridge.h"

#include <fstream>
#include <iostream>
#include <iomanip>

Cartridge::Cartridge(const char* game) {
	// iNES Format Header
	struct sHeader
	{
		char name[4];
		unByte prg_rom_chunks;
		unByte chr_rom_chunks;
		unByte mapper1;
		unByte mapper2;
		unByte prg_ram_size;
		unByte tv_system1;
		unByte tv_system2;
		char unused[5];
	} header;

	std::ifstream ifs;
	ifs.open(game, std::ifstream::binary);
	if (ifs.is_open()) {

		ifs.read((char*)&header, sizeof(sHeader));

		std::cout << header.name[0] << header.name[1] << header.name[2] << std::endl;
		if (header.name[3] == 0x1a) {
			std::cout << "0x1A" << std::endl;
		}
		else {
			std::cout << "Where 0x1A?" << std::endl;
		}

		mapperID = (header.mapper2 & 0xff00) | (header.mapper1 >> 4);

		std::cout << "Mapper ID: " << (int)mapperID << std::endl;

		nPRGBanks = header.prg_rom_chunks;
		nCHRBanks = header.chr_rom_chunks;

		std::cout << "Number of 16 KB PRG-ROM: " << (int)nPRGBanks << std::endl;
		std::cout << "Number of 8 KB VROM: " << (int)nCHRBanks << std::endl;

		PRGMemory.resize(nPRGBanks * 16384);
		ifs.read((char*)PRGMemory.data(), PRGMemory.size());

		if (nCHRBanks == 0) {
			CHRMemory.resize(8192);
		}
		else {
			CHRMemory.resize(nCHRBanks * 8192);
		}
		ifs.read((char*)CHRMemory.data(), CHRMemory.size());
	}
}

unByte Cartridge::CPURead(unWord dir) {
	return PRGMemory[dir & ((nPRGBanks > 1) ? 0x7fff : 0x3fff)];
}

unByte Cartridge::PPURead(unWord dir) {
	return CHRMemory[dir];
}
