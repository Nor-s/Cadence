#include "imguiWindow.h"

#include <imgui.h>

namespace editor
{

ImGuiWindow::ImGuiWindow(const std::string& name) : mName(name)
{
}

void ImGuiWindow::draw()
{
	mOpened = true;

	if (ImGui::Begin(mName.c_str(), &mOpened, mWindowFlags))
	{
		content();
	}
	ImGui::End();
}

}	 // namespace editor
