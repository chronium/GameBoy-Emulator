#pragma once
#include "Gameboy.h"

class Emulator {
public:
	Emulator (Gameboy *gameboy);
	~Emulator ();

	uint8_t Tick ();

	uint8_t NOP ();

	typedef uint8_t (Emulator::*instruction) ();
	static std::map<uint8_t, instruction> instructions;
private:
	Emulator () = delete;
	
	Gameboy *gameboy;
};

