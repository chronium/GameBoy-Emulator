#include "stdafx.h"
#include "Emulator.h"

std::map<uint8_t, Emulator::instruction> Emulator::instructions;

#define HEX_SIGN "%c%x"
#define SIGN_HEX(x) (x < 0)?'-':' ',(x<0)?-x:x

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
	// iHL -> (HL) address in HL

	this->instructions[0x00] = &Emulator::NOP;
	this->instructions[0x05] = &Emulator::DEC_B;
	this->instructions[0x06] = &Emulator::LD_B_d8;
	this->instructions[0x0d] = &Emulator::DEC_C;
	this->instructions[0x0e] = &Emulator::LD_C_d8;
	this->instructions[0x20] = &Emulator::JR_NZ_r8;
	this->instructions[0x21] = &Emulator::LD_HLm_d16; 
	this->instructions[0x32] = &Emulator::LD_iHL_A;
	this->instructions[0x3E] = &Emulator::LD_A_d8;
	this->instructions[0xc3] = &Emulator::JP_d16;
	this->instructions[0xaf] = &Emulator::XOR_A_A;
	this->instructions[0xe0] = &Emulator::LDH_a8_A;
	this->instructions[0xf0] = &Emulator::LDH_A_a8;
	this->instructions[0xf3] = &Emulator::DI;
	this->instructions[0xfe] = &Emulator::CP_d8;
}

Emulator::~Emulator () { }

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

uint8_t Emulator::LD_HLm_d16 () {
	uint16_t nn = gameboy->read_u16 ();
	gameboy->regs.HL.r = nn;
	printf ("LD HL, %x\n", nn);
	return 8;
}

uint8_t Emulator::LD_C_d8 () {
	uint8_t n = gameboy->read_u8 ();
	gameboy->regs.BC.regs.lo = n;
	printf ("LD C, %x\n", n);
	return 8;
}

uint8_t Emulator::LD_B_d8 () {
	uint8_t n = gameboy->read_u8 ();
	gameboy->regs.BC.regs.hi = n;
	printf ("LD B, %x\n", n);
	return 8;
}

uint8_t Emulator::LD_iHL_A () {
	auto a = gameboy->regs.AF.regs.A;
	auto hl = gameboy->regs.HL.r;
	printf ("LD (HL(%x)), A(%x)\n", hl, a);
	gameboy->write_u8 (hl, a);
	gameboy->regs.HL.r--;
	return 8;
}

uint8_t Emulator::DEC_B () {
	auto b = gameboy->regs.BC.regs.hi;
	if (b - 1 < 0) {
		gameboy->regs.BC.regs.hi = 0;
		gameboy->regs.AF.regs.F[4] = 1;
	} else
		gameboy->regs.BC.regs.hi--;
	printf ("DEC B(%x) => %x\n", b, gameboy->regs.BC.regs.hi);
	if (gameboy->regs.BC.regs.hi == 0)
		gameboy->regs.AF.regs.F[7] = 1;
	if (gameboy->regs.BC.regs.hi + 1 > 127)
		gameboy->regs.AF.regs.F[5] = 1;
	return 4;
}

uint8_t Emulator::DEC_C () {
	auto c = gameboy->regs.BC.regs.lo;
	if (c - 1 < 0) {
		gameboy->regs.BC.regs.lo = 0;
		gameboy->regs.AF.regs.F[4] = 1;
	} else
		gameboy->regs.BC.regs.lo--;
	printf ("DEC B(%x) => %x\n", c, gameboy->regs.BC.regs.lo);
	if (gameboy->regs.BC.regs.lo == 0)
		gameboy->regs.AF.regs.F[7] = 1;
	if (gameboy->regs.BC.regs.lo + 1 > 127)
		gameboy->regs.AF.regs.F[5] = 1;
	return 4;
}

uint8_t Emulator::JR_NZ_r8 () {
	int8_t r8 = gameboy->read_s8 ();
	printf ("JR NZ, " HEX_SIGN "\n", SIGN_HEX (r8));
	if (!gameboy->regs.AF.regs.F[7]) {
		gameboy->regs.PC += r8;
		return 12;
	}
	return 8;
}

uint8_t Emulator::LD_A_d8 () {
	uint8_t n = gameboy->read_u8 ();
	gameboy->regs.AF.regs.A = n;
	printf ("LD A, %x\n", n);
	return 8;
}

uint8_t Emulator::DI () {
	gameboy->disable_interrupts = true;
	printf ("DI\n");
	return 4;
}

uint8_t Emulator::LDH_a8_A () {
	uint8_t a8 = gameboy->read_u8 ();
	uint16_t addr = (uint16_t) (0xFF00 + a8);
	printf ("LDH (%x), A(%x)\n", addr, gameboy->regs.AF.regs.A);
	gameboy->write_u8 (addr, gameboy->regs.AF.regs.A);
	return 12;
}

uint8_t Emulator::LDH_A_a8 () {
	uint8_t a8 = gameboy->read_u8 ();
	uint16_t addr = (uint16_t) (0xFF00 + a8);
	printf ("LDH A, (%x)\n", addr);
	gameboy->regs.AF.regs.A = gameboy->read_u8 (addr);
	return 12;
}

uint8_t Emulator::CP_d8 () {
	uint8_t d8 = gameboy->read_u8 ();
	auto a = gameboy->regs.AF.regs.A;

	printf ("CP A(%x), %x\n", a, d8);

	gameboy->regs.AF.regs.F[6] = 1;
	if (a - d8 == 0)
		gameboy->regs.AF.regs.F[7] = 1;
	if (a - d8 > 127)
		gameboy->regs.AF.regs.F[5] = 1;
	if (a < d8)
		gameboy->regs.AF.regs.F[4] = 1;
	return 8;
}

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
