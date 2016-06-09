#pragma once
#include "Gameboy.h"

class Emulator {
public:
	Emulator (Gameboy *gameboy);
	~Emulator ();

	uint8_t Tick ();

	uint8_t NOP ();

	uint8_t LD_BC_d16 ();

	uint8_t DEC_B ();
	uint8_t LD_B_d8 ();

	uint8_t INC_C ();
	uint8_t DEC_C ();
	uint8_t LD_C_d8 ();

	uint8_t LD_DE_d16 ();

	uint8_t RLA ();

	uint8_t LD_A_iDE ();

	uint8_t JR_NZ_r8 ();
	uint8_t LD_HL_d16 ();
	uint8_t LD_iHLi_A ();
	uint8_t INC_HL ();

	uint8_t LD_A_iHLi ();

	uint8_t LD_SP_d16 ();
	uint8_t LD_iHLd_A ();
	uint8_t LD_iHL_d8 ();

	uint8_t LD_A_d8 ();

	uint8_t LD_C_A ();

	uint8_t LD_iHL_A ();

	uint8_t JP_d16 ();

	uint8_t XOR_A_A ();

	uint8_t POP_BC ();

	uint8_t PUSH_BC ();

	uint8_t RET ();

	uint8_t CB ();

	uint8_t CALL_a16 ();

	uint8_t LDH_a8_A ();
	uint8_t LD_iC_A ();

	uint8_t LD_a16_A ();

	uint8_t LDH_A_a8 ();

	uint8_t DI ();

	uint8_t CP_d8 ();

	typedef uint8_t (Emulator::*instruction) ();
	static std::map<uint8_t, instruction> instructions;
private:
	Emulator () = delete;
	
	Gameboy *gameboy;
};

