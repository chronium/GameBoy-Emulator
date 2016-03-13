#include "stdafx.h"
#include "Emulator.h"

std::map<uint8_t, Emulator::instruction> Emulator::instructions;

Emulator::Emulator (Gameboy *gameboy) :
	gameboy(gameboy) {
	this->instructions[0x00] = &Emulator::NOP;		// NOP
	this->instructions[0xc3] = &Emulator::JP_nn;	// JP nn
}

uint8_t Emulator::NOP () {
	printf ("NOP\n");
	return 4;
}

uint8_t Emulator::JP_nn () {
	uint16_t nn = gameboy->read_u16 ();
	gameboy->regs.PC = nn;
	printf ("JP %x\n", nn);
	return 16;
}

Emulator::~Emulator () { }

#define has_instruction(instruction) this->instructions.count (instruction) > 0
#define exec_instruction(instruction) (this->*instructions.at (instruction)) ()

uint8_t Emulator::Tick () {
	printf ("%04x: ", this->gameboy->regs.PC);
	uint8_t opcode = gameboy->read_u8 ();

	if (has_instruction (opcode))
		return exec_instruction (opcode);

	printf ("Unknown opcode %x, halting", opcode);

	return 0xFF;
}
