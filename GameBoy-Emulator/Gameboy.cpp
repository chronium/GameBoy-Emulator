#include "stdafx.h"
#include "Gameboy.h"

std::map<uint16_t, Gameboy::mmio_read> Gameboy::read_mmio;
std::map<uint16_t, Gameboy::mmio_write> Gameboy::write_mmio;

Gameboy::Gameboy (uint8_t *bios, uint8_t *cart, size_t cart_size) { 
	this->memory = new uint8_t[0xFFFF + 1];

	for (int i = 0; i < 0xFFFF; i++)
		this->write_u8 (i, rand ());

	memcpy (this->memory, cart, cart_size > 0x3FFF ? 0x3FFF : cart_size);
	memcpy (this->cart_begin, cart, 0xFF);
	memcpy (this->memory, bios, 0xFF);

	header = (cart_header_t *) &this->memory[0x100];
	
	init_mmio ();
}

Gameboy::~Gameboy () { 
	delete[] memory;
}

void Gameboy::release_bios () {
	static bool released = false;
	if (!released) {
		memcpy (this->memory, this->cart_begin, 0xFF);

		released = true;
	}
}

void Gameboy::init_mmio () {
	this->write_mmio[0xFF0F] = &Gameboy::interrupt_flag;
	this->write_mmio[0xFF40] = &Gameboy::LCDC;

	this->read_mmio[0xFF44] = &Gameboy::LCDC_Y;
}

void Gameboy::interrupt_flag (uint8_t val) {
	if (val & 1) {
		this->interrupt[1] = 1;
	}
}

void Gameboy::LCDC (uint8_t val) {
	if (val & 0x80) {
		screen->FillScreen (0xFFFFFFFF);
	}
}

uint8_t Gameboy::LCDC_Y () {
	return 0;
}

void Gameboy::push_u8 (uint8_t val) {
	this->regs.SP--;
	this->write_u8 (this->regs.SP, val);
}
void Gameboy::push_u16 (uint16_t val) {
	this->push_u8 ((val >> 8) & 0xFF);
	this->push_u8 ((val >> 0) & 0xFF);
}

uint8_t Gameboy::pop_u8 () {
	auto val = this->read_u8 (this->regs.SP);
	this->regs.SP++;
	return val;
}
uint16_t Gameboy::pop_u16 () {
	auto l = this->pop_u8 ();
	auto h = this->pop_u8 ();
	return (h << 8) | l;
}

#define mmio_read_contains(addr) this->read_mmio.count (addr) > 0
#define mmio_write_contains(addr) this->write_mmio.count (addr) > 0

#define mmio_read(addr) (this->*read_mmio.at (addr)) ()
#define mmio_write(addr, val) (this->*write_mmio.at (addr)) (val)

int8_t Gameboy::read_s8 () {
	uint16_t addr = this->regs.PC++;

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

int8_t Gameboy::read_s8 (uint16_t addr) {
	if (mmio_read_contains (addr))
		return (int8_t) mmio_read (addr);

	return (int8_t) this->memory[addr];
}
uint8_t Gameboy::read_u8 (uint16_t addr) {
	if (mmio_read_contains (addr))
		return (uint8_t) mmio_read (addr);

	return (uint8_t) this->memory[addr];
}
int16_t Gameboy::read_s16 (uint16_t addr) {
	if (mmio_read_contains (addr) && mmio_read_contains (addr + 1))
		return (int16_t) (mmio_read (addr) | mmio_read (addr + 1));

	return (int16_t) (this->memory[addr] | (this->memory[addr + 1] << 8));
}
uint16_t Gameboy::read_u16 (uint16_t addr) {
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
