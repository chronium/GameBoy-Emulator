#pragma once
#include "Gameboy.h"

class Emulator {
public:
	Emulator (Gameboy *gameboy);
	~Emulator ();

	uint8_t Tick ();

	uint8_t NOP ();
	
	uint8_t JP_nn ();

	typedef uint8_t (Emulator::*instruction) ();
	static std::map<uint8_t, instruction> instructions;
private:
	Emulator () = delete;
	
	Gameboy *gameboy;
};

