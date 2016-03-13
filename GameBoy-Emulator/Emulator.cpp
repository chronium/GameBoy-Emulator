#include "stdafx.h"
#include "Emulator.h"

std::map<uint8_t, Emulator::instruction> Emulator::instructions;

Emulator::Emulator (Gameboy *gameboy) :
	gameboy(gameboy) {
	this->instructions[0x00] = &Emulator::NOP; // NOP
}

uint8_t Emulator::NOP () {
	printf ("%04x:NOP\n", this->gameboy->regs.PC);
	this->gameboy->regs.PC++;
	return 4;
}

Emulator::~Emulator () { }

#define has_instruction(instruction) this->instructions.count (instruction) > 0
#define exec_instruction(instruction) (this->*instructions.at (instruction)) ()

uint8_t Emulator::Tick () {
	uint8_t opcode = gameboy->read_u8 ();

	if (has_instruction (opcode))
		return exec_instruction (opcode);

	printf ("Unknown opcode %x, halting", opcode);

	return 0xFF;
}
