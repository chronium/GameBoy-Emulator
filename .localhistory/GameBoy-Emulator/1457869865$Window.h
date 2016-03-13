#pragma once
#include <string>
#include <functional>

#include <SDL.h>

class Window {
public:
	Window (const std::string &title, int width, int height);
	~Window ();

	void SetLogicalSize (int width, int height, bool nearest = false);

	void Update (std::function<void ()> func);
	void Render (std::function<void(SDL_Renderer *)> func);

	inline SDL_Renderer *GetRenderer () { return this->renderer; }
	inline void Clear () { SDL_RenderClear (this->GetRenderer ()); }
	inline void Present () { SDL_RenderPresent (this->GetRenderer ()); }
	inline bool IsCloseRequested () { return this->isCloseRequested; }
private:
	Window () = delete;

	int width;
	int height;
	std::string title;
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool isCloseRequested;
};

