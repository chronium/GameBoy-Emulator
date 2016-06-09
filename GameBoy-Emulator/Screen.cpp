#include "stdafx.h"
#include "Screen.h"

#include <fstream>
#include <array>

Screen::Screen (SDL_Renderer *renderer, int width, int height) :
	width (width),
	height (height) {
	this->texture = new DynamicTexture (renderer, this->width, this->height);
}

Screen::~Screen () { }

void Screen::FillScreen (uint32_t color) {
  for (uint32_t i = 0; i < this->texture->GetSize (); i++)
		this->texture->GetPixels ()[i] = color;
}

void Screen::PlotPixel (size_t x, size_t y, uint32_t color) {
	this->texture->GetPixels ()[x + y * this->width] = color;
}
