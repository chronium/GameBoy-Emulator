#include "stdafx.h"
#include "Emulator.h"

std::map<uint8_t, Emulator::instruction> Emulator::instructions;

Emulator::Emulator (Gameboy *gameboy) :
	gameboy(gameboy) {

	// d8  means immediate 8 bit data
	// d16 means immediate 16 bit data
	// a8  means 8 bit unsigned data, which are added to $FF00 in certain instructions (replacement for missing IN and OUT instructions)
	// a16 means 16 bit address
	// r8  means 8 bit signed data, which are added to program counter

	// A, B, C, D, E, H, L -> 8 bit registers
	// AF, BC, DE, HL, SP, PC -> 16 bit registers
	// A -> accumulator
	// F -> flags

	this->instructions[0x00] = &Emulator::NOP;
	this->instructions[0x21] = &Emulator::LD_HL_d16; 
	this->instructions[0xc3] = &Emulator::JP_d16;
	this->instructions[0xaf] = &Emulator::XOR_A_A;
}

uint8_t Emulator::NOP () {
	printf ("NOP\n");
	return 4;
}

uint8_t Emulator::JP_d16 () {
	uint16_t nn = gameboy->read_u16 ();
	gameboy->regs.PC = nn;
	printf ("JP %x\n", nn);
	return 16;
}

uint8_t Emulator::XOR_A_A () {
	auto a = gameboy->regs.AF.regs.A;
	gameboy->regs.AF.regs.A ^= a;
	printf ("XOR A(%x), A(%x) => A(%x)\n", a, a, gameboy->regs.AF.regs.A);
	return 4;
}

uint8_t Emulator::LD_HL_d16 () {
	uint16_t nn = gameboy->read_u16 ();
	gameboy->regs.HL.r = nn;
	printf ("LD HL, %x\n", nn);
	return 8;
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
