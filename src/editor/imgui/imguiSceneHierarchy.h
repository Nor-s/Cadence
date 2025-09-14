#ifndef _EDITOR_IMGUI_IMGUI_HIERARCHY_H_
#define _EDITOR_IMGUI_IMGUI_HIERARCHY_H_

#include <string>
#include "imguiWindow.h"

namespace core
{
class Entity;
}

namespace editor
{

class ImGuiSceneHierarchy : public ImGuiWindow
{
public:
	ImGuiSceneHierarchy() : ImGuiWindow("Hierarchy")
	{
	}

	void content() override;

private:
	void drawTreeNode(const core::Entity& entity);
};

}	 // namespace editor

#endif
