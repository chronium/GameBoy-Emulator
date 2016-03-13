#include "stdafx.h"
#include "Gameboy.h"

std::map<uint16_t, Gameboy::mmio_read *> Gameboy::read_mmio;
std::map<uint16_t, Gameboy::mmio_write *> Gameboy::write_mmio;

Gameboy::Gameboy (uint8_t *cart, size_t cart_size) { 
	this->memory = new uint8_t[0xFFFF + 1];

	for (int i = 0; i < 0xFFFF; i++)
		this->write_u8 (i, rand ());

	memcpy (this->memory, cart, cart_size > 0x3FFF ? 0x3FFF : cart_size);

	header = (cart_header_t *) &this->memory[0x100];

	this->regs.AF.r = 0x01B0;
	this->regs.BC.r = 0x0013;
	this->regs.DE.r = 0x00D8;
	this->regs.HL.r = 0x014D;
	
	this->regs.SP = 0xFFFE;
	this->regs.PC = 0x0100;

	this->write_u8(0xFF05, 0x00);
	this->write_u8(0xFF06, 0x00);
	this->write_u8(0xFF07, 0x00);
	this->write_u8(0xFF10, 0x80);
	this->write_u8(0xFF11, 0xBF);
	this->write_u8(0xFF12, 0xF3);
	this->write_u8(0xFF14, 0xBF);
	this->write_u8(0xFF16, 0x3F);
	this->write_u8(0xFF17, 0x00);
	this->write_u8(0xFF19, 0xBF);
	this->write_u8(0xFF1A, 0x7F);
	this->write_u8(0xFF1B, 0xFF);
	this->write_u8(0xFF1C, 0x9F);
	this->write_u8(0xFF1E, 0xBF);
	this->write_u8(0xFF20, 0xFF);
	this->write_u8(0xFF21, 0x00);
	this->write_u8(0xFF22, 0x00);
	this->write_u8(0xFF23, 0xBF);
	this->write_u8(0xFF24, 0x77);
	this->write_u8(0xFF25, 0xF3);
	this->write_u8(0xFF26, 0xF1);
	this->write_u8(0xFF40, 0x91);
	this->write_u8(0xFF42, 0x00);
	this->write_u8(0xFF43, 0x00);
	this->write_u8(0xFF45, 0x00);
	this->write_u8(0xFF47, 0xFC);
	this->write_u8(0xFF48, 0xFF);
	this->write_u8(0xFF49, 0xFF);
	this->write_u8(0xFF4A, 0x00);
	this->write_u8(0xFF4B, 0x00);
	this->write_u8(0xFFFF, 0x00);
}

Gameboy::~Gameboy () { 
	delete[] memory;
}

#define mmio_read_contains(addr) this->read_mmio.count (addr) > 0
#define mmio_write_contains(addr) this->write_mmio.count (addr) > 0

#define mmio_read(addr) (*this->read_mmio.at (addr)) (addr)
#define mmio_write(addr, val) (*this->write_mmio.at (addr)) (addr, val)

int8_t Gameboy::read_s8 () {
	uint16_t addr = ++this->regs.PC;

	if (mmio_read_contains (addr))
		return (int8_t) mmio_read (addr);

	return (int8_t) this->memory[addr];
}
uint8_t Gameboy::read_u8 () {
	uint16_t addr = this->regs.PC++;

	if (mmio_read_contains (addr))
		return (uint8_t) mmio_read (addr);

	return (uint8_t) this->memory[addr];
}
int16_t Gameboy::read_s16 () {
	uint16_t addr = this->regs.PC;
	this->regs.PC += 2;

	if (mmio_read_contains (addr) && mmio_read_contains (addr + 1))
		return (int16_t) (mmio_read (addr) | mmio_read (addr + 1));

	return (int16_t) (this->memory[addr] | (this->memory[addr + 1] << 8));
}
uint16_t Gameboy::read_u16 () {
	uint16_t addr = this->regs.PC;
	this->regs.PC += 2;

	if (mmio_read_contains (addr) && mmio_read_contains (addr + 1))
		return (uint16_t) (mmio_read (addr) | (mmio_read (addr)));

	return (uint16_t) (this->memory[addr] | (this->memory[addr + 1] << 8));
}

void Gameboy::write_s8 (uint16_t addr, int8_t val) {
	this->memory[addr] = val;

	if (mmio_write_contains (addr))
		mmio_write (addr, val);
}
void Gameboy::write_u8 (uint16_t addr, uint8_t val) {
	this->memory[addr] = val;

	if (mmio_write_contains (addr))
		mmio_write (addr, val);
}
void Gameboy::write_s16 (uint16_t addr, int16_t val) {
	this->memory[addr + 0] = val & 0xFF00;
	this->memory[addr + 1] = val & 0x00FF;

	if (mmio_write_contains (addr) && mmio_write_contains (addr + 1)) {
		mmio_write (addr + 0, val & 0xFF00);
		mmio_write (addr + 1, val & 0x00FF);
	}
}
void Gameboy::write_u16 (uint16_t addr, uint16_t val) {
	this->memory[addr + 0] = val & 0xFF00;
	this->memory[addr + 1] = val & 0x00FF;

	if (mmio_write_contains (addr) && mmio_write_contains (addr + 1)) {
		mmio_write (addr + 0, val & 0xFF00);
		mmio_write (addr + 1, val & 0x00FF);
	}
}

uint8_t Gameboy::header_checksum (uint8_t *mem) {
	uint8_t checksum = 0;
	for (size_t i = 0x134; i <= 0x14c; i++)
		checksum = checksum - mem[i] - 1;
	return checksum;
}
