#include "stdafx.h"
#include "Emulator.h"

std::map<uint8_t, Emulator::instruction> Emulator::instructions;

#define HEX_SIGN "%c%x"
#define SIGN_HEX(x) (x < 0)?'-':' ',(x<0)?-x:x

Emulator::Emulator (Gameboy *gameboy) :
	gameboy(gameboy) {

	// d8  -> immediate 8 bit data
	// d16 -> immediate 16 bit data
	// a8  -> 8 bit unsigned data, which are added to $FF00 in certain instructions (replacement for missing IN and OUT instructions)
	// a16 -> 16 bit address
	// r8  -> 8 bit signed data, which is added to program counter

	// A, B, C, D, E, H, L -> 8 bit registers
	// AF, BC, DE, HL, SP, PC -> 16 bit registers
	// A -> accumulator
	// F -> flags
	// iA, iB, iC, iD, iE, iH, iL -> 8 bit indirect register
	// iAF, iBC, iDE, iHL -> 8 bit indirect register

	this->instructions[0x00] = &Emulator::NOP;
	this->instructions[0x01] = &Emulator::LD_BC_d16;
	this->instructions[0x05] = &Emulator::DEC_B;
	this->instructions[0x06] = &Emulator::LD_B_d8;
	this->instructions[0x0c] = &Emulator::INC_C;
	this->instructions[0x0d] = &Emulator::DEC_C;
	this->instructions[0x0e] = &Emulator::LD_C_d8;
	this->instructions[0x11] = &Emulator::LD_DE_d16;
	this->instructions[0x17] = &Emulator::RLA;
	this->instructions[0x1a] = &Emulator::LD_A_iDE;
	this->instructions[0x20] = &Emulator::JR_NZ_r8;
	this->instructions[0x21] = &Emulator::LD_HL_d16;
	this->instructions[0x22] = &Emulator::LD_iHLi_A;
	this->instructions[0x23] = &Emulator::INC_HL;
	this->instructions[0x2a] = &Emulator::LD_A_iHLi;
	this->instructions[0x31] = &Emulator::LD_SP_d16;
	this->instructions[0x32] = &Emulator::LD_iHLd_A;
	this->instructions[0x36] = &Emulator::LD_iHL_d8;
	this->instructions[0x3e] = &Emulator::LD_A_d8;
	this->instructions[0x4f] = &Emulator::LD_C_A;
	this->instructions[0x77] = &Emulator::LD_iHL_A;
	this->instructions[0xc3] = &Emulator::JP_d16;
	this->instructions[0xaf] = &Emulator::XOR_A_A;
	this->instructions[0xc1] = &Emulator::POP_BC;
	this->instructions[0xc5] = &Emulator::PUSH_BC;
	this->instructions[0xc9] = &Emulator::RET;
	this->instructions[0xcb] = &Emulator::CB;
	this->instructions[0xcd] = &Emulator::CALL_a16;
	this->instructions[0xe0] = &Emulator::LDH_a8_A;
	this->instructions[0xe2] = &Emulator::LD_iC_A;
	this->instructions[0xea] = &Emulator::LD_a16_A;
	this->instructions[0xf0] = &Emulator::LDH_A_a8;
	this->instructions[0xf3] = &Emulator::DI;
	this->instructions[0xfe] = &Emulator::CP_d8;
}

Emulator::~Emulator () { }

uint8_t Emulator::NOP () {
	printf ("NOP\n");
	return 4;
}

uint8_t Emulator::LD_BC_d16 () {
	auto d16 = gameboy->read_u16 ();
	printf ("LD BC, %04x\n", d16);
	gameboy->regs.BC.r = d16;
	return 12;
}

uint8_t Emulator::DEC_B () {
	auto b = gameboy->regs.BC.regs.hi;
	if (b - 1 < 0) {
		gameboy->regs.BC.regs.hi = 0;
		gameboy->regs.AF.regs.F[4] = 1;
	} else
		gameboy->regs.BC.regs.hi--;
	printf ("DEC B(%02x) => %02x\n", b, gameboy->regs.BC.regs.hi);
	if (gameboy->regs.BC.regs.hi == 0)
		gameboy->regs.AF.regs.F[7] = 1;
	if (gameboy->regs.BC.regs.hi > 127)
		gameboy->regs.AF.regs.F[5] = 1;
	return 4;
}

uint8_t Emulator::INC_C () {
	auto c = gameboy->regs.BC.regs.lo;
	if (c + 1 > 256) {
		gameboy->regs.BC.regs.lo = 0;
		gameboy->regs.AF.regs.F[4] = 1;
	} else
		gameboy->regs.BC.regs.lo++;
	printf ("INC C(%02x) => %02x\n", c, gameboy->regs.BC.regs.lo);
	if (gameboy->regs.BC.regs.lo == 0)
		gameboy->regs.AF.regs.F[7] = 1;
	if (gameboy->regs.BC.regs.lo > 127)
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
	printf ("DEC C(%02x) => %02x\n", c, gameboy->regs.BC.regs.lo);
	if (gameboy->regs.BC.regs.lo == 0)
		gameboy->regs.AF.regs.F[7] = 1;
	if (gameboy->regs.BC.regs.lo > 127)
		gameboy->regs.AF.regs.F[5] = 1;
	return 4;
}

uint8_t Emulator::LD_HL_d16 () {
	uint16_t nn = gameboy->read_u16 ();
	gameboy->regs.HL.r = nn;
	printf ("LD HL, %02x\n", nn);
	return 8;
}

uint8_t Emulator::RLA () {
	auto a = gameboy->regs.AF.regs.A;
	uint8_t a7 = (a >> 6) & 0xb1;
	uint8_t ca = gameboy->regs.AF.regs.F[4];

	printf ("RLA %02x\n", a);
	a <<= 1;
	a |= ca;

	gameboy->regs.AF.regs.F[4] = a7;

	if (a == 0)
		gameboy->regs.AF.regs.F[7] = 1;
	else
		gameboy->regs.AF.regs.F[0] = 1;

	gameboy->regs.AF.regs.F[6] = 0;
	gameboy->regs.AF.regs.F[5] = 0;

	gameboy->regs.AF.regs.A = a;

	return 4;
}

uint8_t Emulator::LD_A_iHLi () {
	auto a = gameboy->regs.AF.regs.A;
	auto hl = gameboy->regs.HL.r;

	printf ("LD A, (HL(%04x)+)\n", hl);
	gameboy->regs.AF.regs.A = gameboy->read_u8 (gameboy->regs.HL.r++);

	return 8;
}

uint8_t Emulator::LD_C_d8 () {
	uint8_t n = gameboy->read_u8 ();
	gameboy->regs.BC.regs.lo = n;
	printf ("LD C, %02x\n", n);
	return 8;
}

uint8_t Emulator::LD_DE_d16 () {
	auto d16 = gameboy->read_u16 ();
	printf ("LD DE, %04x\n", d16);
	gameboy->regs.DE.r = d16;
	return 12;
}

uint8_t Emulator::LD_A_iDE () {
	auto de = gameboy->regs.DE.r;

	printf ("LD A, (DE(%04x))\n", de);
	gameboy->regs.AF.regs.A = gameboy->read_u8 (de);

	return 8;
}

uint8_t Emulator::LD_B_d8 () {
	uint8_t n = gameboy->read_u8 ();
	gameboy->regs.BC.regs.hi = n;
	printf ("LD B, %02x\n", n);
	return 8;
}

uint8_t Emulator::LD_SP_d16 () {
	uint16_t nn = gameboy->read_u16 ();
	
	printf ("LD SP, %04x\n", nn);
	gameboy->regs.SP = nn;

	return 12;
}

uint8_t Emulator::LD_iHLd_A () {
	auto a = gameboy->regs.AF.regs.A;
	auto hl = gameboy->regs.HL.r;
	printf ("LD (HL(%04x)-), A(%02x)\n", hl, a);
	gameboy->write_u8 (hl, a);
	gameboy->regs.HL.r--;
	return 8;
}

uint8_t Emulator::LD_iHL_d8 () {
	uint8_t n = gameboy->read_u8 ();
	auto hl = gameboy->regs.HL.r;

	printf ("LD (HL(%04x)), %02x\n", hl, n);
	gameboy->write_u8 (hl, n);

	return 12;
}

uint8_t Emulator::LD_iHL_A () {
	auto a = gameboy->regs.AF.regs.A;
	auto hl = gameboy->regs.HL.r;

	printf ("LD (HL(%04x)), A(%02x)\n", hl, a);
	gameboy->write_u8 (hl, a);

	return 8;
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

uint8_t Emulator::LD_iHLi_A () {
	auto a = gameboy->regs.AF.regs.A;
	auto hl = gameboy->regs.HL.r;

	printf ("LD (HL(%04x)+), A(%02x)\n", hl, a);

	gameboy->write_u16 (hl, a);
	gameboy->regs.HL.r++;

	return 8;
}

uint8_t Emulator::INC_HL () {
	auto hl = gameboy->regs.HL.r;

	printf ("INC HL(%04x)\n", hl);
	
	gameboy->regs.HL.r++;

	return 8;
}

uint8_t Emulator::LD_A_d8 () {
	uint8_t n = gameboy->read_u8 ();
	gameboy->regs.AF.regs.A = n;
	printf ("LD A, %02x\n", n);
	return 8;
}

uint8_t Emulator::LD_C_A () {
	auto a = gameboy->regs.AF.regs.A;
	
	printf ("LD C, A(%02x)\n", a);
	gameboy->regs.BC.regs.lo = a;
	
	return 4;
}

uint8_t Emulator::JP_d16 () {
	uint16_t nn = gameboy->read_u16 ();
	gameboy->regs.PC = nn;
	printf ("JP %02x\n", nn);
	return 16;
}

uint8_t Emulator::XOR_A_A () {
	auto a = gameboy->regs.AF.regs.A;
	gameboy->regs.AF.regs.A ^= a;
	printf ("XOR A(%02x), A(%02x) => A(%02x)\n", a, a, gameboy->regs.AF.regs.A);
	return 4;
}

uint8_t Emulator::POP_BC () {
	auto bc = gameboy->pop_u16 ();
	printf ("POP BC(%04x)\n", bc);

	gameboy->regs.BC.r = bc;

	return 16;
}

uint8_t Emulator::PUSH_BC () {
	auto bc = gameboy->regs.BC.r;
	printf ("PUSH BC(%04x)\n", bc);
	gameboy->push_u16 (bc);
	return 16;
}

uint8_t Emulator::RET () {
	auto addr = gameboy->pop_u16 ();
	printf ("RET %04x\n", addr);

	gameboy->regs.PC = addr;

	return 16;
}

uint8_t Emulator::CB () {
	uint8_t time = 4;
	
	auto opcode = gameboy->read_u8 ();

	switch (opcode) {
		case 0x11:
		{
			auto c = gameboy->regs.BC.regs.lo;
			uint8_t c7 = (c >> 6) & 0b1;
			uint8_t ca = gameboy->regs.AF.regs.F[4];

			printf ("RL C\n");
			c <<= 1;
			c |= ca;
			gameboy->regs.AF.regs.F[4] = c7;

			gameboy->regs.BC.regs.lo = c;

			if (c == 0)
				gameboy->regs.AF.regs.F[7] = 1;
			else
				gameboy->regs.AF.regs.F[0] = 1;

			gameboy->regs.AF.regs.F[6] = 0;
			gameboy->regs.AF.regs.F[5] = 0;

			return 8;
		}
		case 0x7c:
		{
			auto h = gameboy->regs.HL.regs.hi;
			printf ("BIT 7, H(%02x)\n", h);
			uint8_t bit = (h >> 6) & 0b1;

			if (bit == 0)
				gameboy->regs.AF.regs.F[7] = 1;

			gameboy->regs.AF.regs.F[6] = 0;
			gameboy->regs.AF.regs.F[5] = 1;

			time += 4;
			break;
		}
		default:
			printf ("Unhandled CB: %02x\n", opcode);
			return 0xFF;
	}

	return time;
}

uint8_t Emulator::CALL_a16 () {
	auto addr = gameboy->read_u16 ();
	gameboy->push_u16 (gameboy->regs.PC);
	printf ("CALL %04x\n", addr);
	gameboy->regs.PC = addr;
	return 24;
}

uint8_t Emulator::DI () {
	gameboy->disable_interrupts = true;
	printf ("DI\n");
	return 4;
}

uint8_t Emulator::LDH_a8_A () {
	uint8_t a8 = gameboy->read_u8 ();
	uint16_t addr = (uint16_t) (0xFF00 + a8);
	printf ("LDH (ff00 + %02x), A(%02x)\n", a8, gameboy->regs.AF.regs.A);
	gameboy->write_u8 (addr, gameboy->regs.AF.regs.A);
	return 12;
}

uint8_t Emulator::LD_iC_A () {
	auto a = gameboy->regs.AF.regs.A;
	auto c = gameboy->regs.BC.regs.lo;
	auto addr = (uint16_t) (0xFF00 + c);

	printf ("LD C(ff00 + %02x), A(%02x)\n", c, a);
	gameboy->write_u8 (addr, a);

	return 8;
}

uint8_t Emulator::LD_a16_A () {
	uint16_t nn = gameboy->read_u16 ();
	auto a = gameboy->regs.AF.regs.A;

	printf ("LD (%04x), A(%02x)\n", nn, a);
	gameboy->write_u8 (nn, a);
	return 16;
}

uint8_t Emulator::LDH_A_a8 () {
	uint8_t a8 = gameboy->read_u8 ();
	uint16_t addr = (uint16_t) (0xFF00 + a8);
	printf ("LDH A, (ff00 + %02x)\n", a8);
	gameboy->regs.AF.regs.A = gameboy->read_u8 (addr);
	return 12;
}

uint8_t Emulator::CP_d8 () {
	uint8_t d8 = gameboy->read_u8 ();
	auto a = gameboy->regs.AF.regs.A;

	printf ("CP A(%02x), %02x\n", a, d8);

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

	printf ("Unknown opcode %02x, halting", opcode);

	return 0xFF;
}
