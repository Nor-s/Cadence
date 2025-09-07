#ifndef _WINDOW_GLWINDOW_H_
#define _WINDOW_GLWINDOW_H_

#include "window.h"

#include <string>

namespace editor
{

class GLWindow : public Window
{
public:
	GLWindow(const core::Size& res);
	~GLWindow();

	void update() override;
	void refresh() override;
	void drawend() override;

private:
	std::string mGlslVersion;
};

}	 // namespace editor

#endif
