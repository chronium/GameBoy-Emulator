#include "stdafx.h"
#include "DynamicTexture.h"


DynamicTexture::DynamicTexture (SDL_Renderer *renderer, int width, int height) :
	width (width),
	height (height) {
	pixels = new uint32_t[this->width * this->height];
	memset (pixels, 0, this->width* this->height * sizeof (uint32_t));

	this->texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, this->width, this->height);
}

DynamicTexture::~DynamicTexture () { 
	delete[] this->pixels;
	SDL_DestroyTexture (this->texture);
}

void DynamicTexture::Draw (SDL_Renderer *renderer) {
	SDL_RenderCopy (renderer, this->texture, NULL, NULL);
}

void DynamicTexture::Update () {
	SDL_UpdateTexture (this->texture, NULL, pixels, width * sizeof (uint32_t));
}
