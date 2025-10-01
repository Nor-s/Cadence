#include "window.h"
namespace editor
{

Window::Window()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		mInitialized = false;
		SDL_Log("Could not initialize SDL: %s", SDL_GetError());
		return;
	}

	mInitialized = true;
	mWindow = nullptr;
}

Window::~Window()
{
	if (mWindow)
	{
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}

	if (mInitialized)
	{
		SDL_Quit();
		mInitialized = false;
	}
}

void Window::show()
{
	SDL_ShowWindow(mWindow);
	refresh();
}

}	 // namespace editor