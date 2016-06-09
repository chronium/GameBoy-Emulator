#pragma once
#include <stdint.h>
#include <stddef.h>

#include <map>
#include <bitset>
#include <functional>

#include "Screen.h"

class Gameboy {
#define ROM_SIZE(N) (32000 << N)
#define CHECK_HEADER(mem, header) header_checksum ((uint8_t *) mem) == header->header_checksum

	typedef struct cart_header {
		union {
			struct entry {
				uint8_t nop;
				uint8_t jmp;
				uint16_t addr;
			} entry;
			uint32_t entry_pt;
		} entry_point;
		uint8_t nintendo_logo[0x30];
		char title[11];
		char manufacturer_code[4];
		uint8_t cgb_flag;
		char license_code[2];
		uint8_t sgb_flag;
		uint8_t cartridge_type;
		uint8_t rom_size;
		uint8_t ram_size;
		uint8_t dest_code;
		uint8_t old_license_code;
		uint8_t mask_version_number;
		uint8_t header_checksum;
		uint16_t global_checksum;
	} cart_header_t;

	uint8_t header_checksum (uint8_t *mem);

	typedef union reg {
		uint16_t r;
		struct regs { 
			uint8_t hi;
			uint8_t lo;
		} regs;
		
		reg () { r = 0; }
	} reg_t;

	typedef struct registers {
		union AF {
			uint16_t r;
			struct regs {
				uint8_t A;
				std::bitset<8> F;
      public:
        regs () { A = 0; }
			} regs;

			AF () { r = 0; }
		} AF;
		reg_t BC;
		reg_t DE;
		reg_t HL;
		uint16_t SP;
		uint16_t PC;
  public:
    registers () { }
  } registers_t;
public:
	Gameboy (uint8_t *bios, uint8_t *cart, size_t cart_size);
	~Gameboy ();

	inline void SetScreen (Screen *screen) { this->screen = screen; }

	void release_bios ();

	int8_t read_s8 ();
	uint8_t read_u8 ();
	int16_t read_s16 ();
	uint16_t read_u16 ();

	int8_t read_s8 (uint16_t addr);
	uint8_t read_u8 (uint16_t addr);
	int16_t read_s16 (uint16_t addr);
	uint16_t read_u16 (uint16_t addr);

	void push_u8 (uint8_t val);
	void push_u16 (uint16_t val);

	uint8_t pop_u8 ();
	uint16_t pop_u16 ();

	void write_s8 (uint16_t addr, int8_t val);
	void write_u8 (uint16_t addr, uint8_t val);
	void write_s16 (uint16_t addr, int16_t val);
	void write_u16 (uint16_t addr, uint16_t val);

	typedef uint8_t (Gameboy::*mmio_read) ();
	typedef void (Gameboy::*mmio_write) (uint8_t val);

	void interrupt_flag (uint8_t val);
	void LCDC (uint8_t val);
	
	uint8_t LCDC_Y ();

	void init_mmio ();

	registers_t regs;
	cart_header_t *header;

	uint8_t cart_begin[0xFF];

	bool disable_interrupts = false;
private:
	Gameboy () = delete;

	uint8_t *memory;

	static std::map<uint16_t, mmio_read> read_mmio;
	static std::map<uint16_t, mmio_write> write_mmio;

	std::bitset<8> interrupt;

	Screen *screen;
};

