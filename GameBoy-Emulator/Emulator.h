#pragma once
#include "Gameboy.h"

class Emulator {
public:
	Emulator (Gameboy *gameboy);
	~Emulator ();

	uint8_t Tick ();

	uint8_t NOP (); // 0x00
	
	uint8_t LD_hl_nn (); // 0x21

	uint8_t JP_nn (); // 0xc3

	uint8_t XOR_a_a (); // 0xaf

	typedef uint8_t (Emulator::*instruction) ();
	static std::map<uint8_t, instruction> instructions;
private:
	Emulator () = delete;
	
	Gameboy *gameboy;
};

