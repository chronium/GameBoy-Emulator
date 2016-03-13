#pragma once
#include "Gameboy.h"

class Emulator {
public:
	Emulator (Gameboy *gameboy);
	~Emulator ();

	uint8_t Tick ();
private:
	Emulator () = delete;
	
	Gameboy *gameboy;
};

