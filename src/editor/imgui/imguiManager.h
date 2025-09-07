#ifndef _EDITOR_IMGUI_IMGUI_MANAGER_H_
#define _EDITOR_IMGUI_IMGUI_MANAGER_H_

#include <vector>

namespace core
{
class CanvasWrapper;
}

namespace editor
{

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();
	void update();
	void draw();
	void drawCanvas(std::vector<core::CanvasWrapper*>& canvasList);
	void drawend();

private:
	void init();
	void drawDocSpace();
	void drawDocMenuBar();
};

}	 // namespace editor

#endif