#include "NES.h"

#include <Windows.h>
#include <iostream>

int main(void) {

	NES nes;

	//nes.Load("nestest.nes");
	nes.Load("Donkey Kong.nes");

	//while (!(GetKeyState('A') & 0x8000)) {
	//	nes.Clock();
	//}

	nes.PourMemory();

	return 0;
}