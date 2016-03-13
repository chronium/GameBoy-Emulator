#include "stdafx.h"
#include "Emulator.h"


Emulator::Emulator (Gameboy *gameboy) :
	gameboy(gameboy) {
	gameboy->instructions[0x00] = &NOP; // NOP
}

uint8_t NOP (Gameboy *gb) {
	printf ("%04x:NOP\n", gb->regs.PC);
	gb->regs.PC++;
	return 4;
}

Emulator::~Emulator () { }

#define has_instruction(instruction) gameboy->instructions.count (instruction) > 0
#define exec_instruction(instruction) gameboy->instructions.at (instruction) (gameboy)

uint8_t Emulator::Tick () {
	uint8_t opcode = gameboy->read_u8 ();

	if (has_instruction (opcode))
		return exec_instruction (opcode);

	printf ("Unknown opcode %x, halting", opcode);

	return 0xFF;
}
