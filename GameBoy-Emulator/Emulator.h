#pragma once
#include "Gameboy.h"

class Emulator {
public:
	Emulator (Gameboy *gameboy);
	~Emulator ();

	uint8_t Tick ();

	uint8_t NOP ();

	uint8_t DEC_B ();
	uint8_t LD_B_d8 ();
	
	uint8_t DEC_C ();
	uint8_t LD_C_d8 ();

	uint8_t JR_NZ_r8 ();
	uint8_t LD_HLm_d16 ();

	uint8_t LD_iHL_A ();

	uint8_t LD_A_d8 ();

	uint8_t JP_d16 ();

	uint8_t XOR_A_A ();

	uint8_t LDH_a8_A ();

	uint8_t DI ();

	typedef uint8_t (Emulator::*instruction) ();
	static std::map<uint8_t, instruction> instructions;
private:
	Emulator () = delete;
	
	Gameboy *gameboy;
};

