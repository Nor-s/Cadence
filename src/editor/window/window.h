#ifndef _EDITOR_WINDOW_WINDOW_H_
#define _EDITOR_WINDOW_WINDOW_H_

#include <core/core.h>
#include <SDL.h>

namespace editor
{

class Window
{
public:
	Window();
	virtual ~Window();

	void show();
	virtual void update(){};
	virtual void refresh(){};
	virtual void drawend(){};

	SDL_Window* mWindow = nullptr;
	SDL_GLContext mContext = nullptr;
	bool mInitialized = false;
};

}	 // namespace editor

#endif