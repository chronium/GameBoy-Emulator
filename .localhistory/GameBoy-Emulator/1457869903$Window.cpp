#include "stdafx.h"
#include "Window.h"


Window::Window (const std::string &title, int width, int height) : 
	width (width),
	height (height),
	title (title) {
	SDL_Init (SDL_INIT_EVERYTHING);

	this->window = SDL_CreateWindow (title.c_str (), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, NULL);
	this->renderer = SDL_CreateRenderer (this->window, -1, 0);
}

void Window::SetLogicalSize (int width, int height, bool nearest) {
	if (!nearest)
		SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	else
		SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	SDL_RenderSetLogicalSize (this->renderer, width, height);
}

void Window::Update (std::function<void ()> func) {
	SDL_Event e;
	while (SDL_PollEvent (&e)) {
		if (e.type == SDL_QUIT)
			this->isCloseRequested = true;
	}

	func ();
}
void Window::Render (std::function<void (SDL_Renderer *)> func) {
	this->Clear ();
	func (this->renderer);
	this->Present ();
}

Window::~Window () { 
	SDL_DestroyRenderer (this->renderer);
	SDL_DestroyWindow (this->window);
	SDL_Quit ();
}
