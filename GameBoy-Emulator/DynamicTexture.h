#pragma once
#include <stdint.h>
#include <SDL.h>

class DynamicTexture {
public:
	DynamicTexture (SDL_Renderer *renderer, int width, int height);
	~DynamicTexture ();

	void Update ();
	void Draw (SDL_Renderer *renderer);
private:
	DynamicTexture () = delete;

	int width;
	int height;

	SDL_Texture *texture;
	uint32_t *pixels;
};

