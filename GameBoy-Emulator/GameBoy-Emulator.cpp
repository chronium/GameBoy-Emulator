#define SDL_MAIN_HANDLED

#include "stdafx.h"

#include <cstdio>
#include <cstring>

#include "Gameboy.h"
#include "Window.h"
#include "DynamicTexture.h"
#include "Emulator.h"

#include <fstream>
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <SDL.h>

#define WIDTH  160
#define HEIGHT 144
#define SCALE  4

template<typename ... Args>
std::string string_format (const std::string& format, Args ... args) {
	size_t size = snprintf (nullptr, 0, format.c_str (), args ...) + 1;
	std::unique_ptr<char[]> buf (new char[size]);
	snprintf (buf.get (), size, format.c_str (), args ...);
	return std::string (buf.get (), buf.get () + size - 1);
}

int main(int argc, char *args[]) {
	std::ifstream cart (args[2], std::ios::binary);
	std::vector<char> cart_data ((std::istreambuf_iterator<char> (cart)),
								 (std::istreambuf_iterator<char> ()));
	std::ifstream bios (args[1], std::ios::binary);
	std::vector<char> bios_data ((std::istreambuf_iterator<char> (bios)),
		(std::istreambuf_iterator<char> ()));

	Gameboy *gameboy = new Gameboy ((uint8_t *) bios_data.data (), (uint8_t *) cart_data.data (), cart_data.size ());

	Window *window = new Window (string_format("Gameboy Emulator - Cart: %s", gameboy->header->title), WIDTH * SCALE, HEIGHT * SCALE);
	window->SetLogicalSize (WIDTH, HEIGHT, true);

	Screen *screen = new Screen (window->GetRenderer (), WIDTH, HEIGHT);
	gameboy->SetScreen (screen);

	Emulator *emulator = new Emulator (gameboy);

	bool halted = false;
	int ticks = 0;

	auto updateFunc = [&] () {
		screen->GetTexture()->Update ();

		if (!halted) {
			if (gameboy->regs.PC == 0x100)
				gameboy->release_bios ();
			auto tick = emulator->Tick ();
			halted = tick == 0xFF;
			ticks += tick;
		}
	};

	auto renderfunc = [&] (SDL_Renderer *renderer) {
		screen->GetTexture ()->Draw (renderer);
	};

	window->EnterLoop (updateFunc, renderfunc);

	delete screen;
	delete window;

    return 0;
}

