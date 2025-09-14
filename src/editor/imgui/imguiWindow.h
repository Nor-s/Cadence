#ifndef _EDITOR_IMGUI_IMGUI_WINDOW_H_
#define _EDITOR_IMGUI_IMGUI_WINDOW_H_

#include <string>

namespace editor
{

class ImGuiWindow
{
public:
	ImGuiWindow(const std::string& name);
	virtual ~ImGuiWindow() = default;

	virtual void draw();
	virtual void content() = 0;

protected:
	std::string mName;
	bool mOpened = true;
};

}	 // namespace editor

#endif
