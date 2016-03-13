// GameBoy-Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Gameboy.h"
#include "Window.h"
#include "DynamicTexture.h"

#include <string>
#include <bitset>

#include <SDL.h>

int main(int argc, char *args[])
{
	bool leftMouseButtonDown = false;
	bool quit = false;
	SDL_Event event;

	int width = 160;
	int height = 144;
	int scale = 4;

	Window *window = new Window ("Gameboy Emulator", width * scale, height * scale);
	window->SetLogicalSize (width, height, true);

	DynamicTexture *screen = new DynamicTexture (window->GetRenderer (), width, height);

	int cycles = 0;

	while (!quit) {
		screen->Update ();
		SDL_WaitEvent (&event);

		switch (event.type) {
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = true;
			case SDL_MOUSEMOTION:
				if (leftMouseButtonDown) {
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
		}

		window->Render ([=] (SDL_Renderer *renderer) {
			screen->Draw (renderer);
		});
	}

	delete screen;
	delete window;
	SDL_Quit ();

    return 0;
}

