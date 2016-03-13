#pragma once
#include "Gameboy.h"

uint8_t NOP (Gameboy *gb);

class Emulator {
public:
	Emulator (Gameboy *gameboy);
	~Emulator ();

	uint8_t Tick ();
private:
	Emulator () = delete;
	
	Gameboy *gameboy;
};

