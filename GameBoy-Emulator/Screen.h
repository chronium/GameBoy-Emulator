#pragma once
#include "DynamicTexture.h"

class Screen {
public:
	Screen (SDL_Renderer *renderer, int width, int height);
	~Screen ();

	DynamicTexture *GetTexture () { return this->texture; }

	void FillScreen (uint32_t color);
	void PlotPixel (size_t x, size_t y, uint32_t color);
private:
	Screen ();

	int width;
	int height;

	DynamicTexture *texture;
};

