#include "NES.h"
#include "Cartridge.h"

#include <Windows.h>
#include <iostream>

int main(void) {

	//Cartridge *cart = new Cartridge("nestest.nes");
	Cartridge* cart = new Cartridge("Donkey Kong.nes");

	NES nes;

	nes.InsertCart(cart);

	//while (!(GetKeyState('A') & 0x8000)) {
	//	nes.Clock();
	//}

	nes.PourMemory();

	return 0;
}