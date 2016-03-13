// GameBoy-Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Gameboy.h"
#include "Window.h"
#include "DynamicTexture.h"

#include <string>
#include <bitset>

#include <SDL.h>

int main(int argc, char *args[]) {

	int width = 160;
	int height = 144;
	int scale = 4;

	Window *window = new Window ("Gameboy Emulator", width * scale, height * scale);
	window->SetLogicalSize (width, height, true);

	DynamicTexture *screen = new DynamicTexture (window->GetRenderer (), width, height);

	int cycles = 0;

	auto updateFunc = [=] () {
		screen->Update ();
	};
	auto renderfunc = [] (SDL_Renderer *renderer) {
		screen->draw (renderer);
	};

	while (!window->IsCloseRequested ()) {
		screen->Update ();
		window->Update ([=] (SDL_Renderer *renderer) {
		});

		window->Render ([=] (SDL_Renderer *renderer) {
			screen->Draw (renderer);
		});
	}

	delete screen;
	delete window;

    return 0;
}

