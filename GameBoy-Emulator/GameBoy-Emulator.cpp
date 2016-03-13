#include "stdafx.h"

#include "Gameboy.h"
#include "Window.h"
#include "DynamicTexture.h"

#include <SDL.h>

#define WIDTH  160
#define HEIGHT 144
#define SCALE  4

int main(int argc, char *args[]) {
	Window *window = new Window ("Gameboy Emulator", WIDTH * SCALE, HEIGHT * SCALE);
	window->SetLogicalSize (WIDTH, HEIGHT, true);

	DynamicTexture *screen = new DynamicTexture (window->GetRenderer (), WIDTH, HEIGHT);

	auto updateFunc = [=] () {
		screen->Update ();
	};
	auto renderfunc = [=] (SDL_Renderer *renderer) {
		screen->Draw (renderer);
	};

	window->EnterLoop (updateFunc, renderfunc);

	delete screen;
	delete window;

    return 0;
}

