#include "imguiContentBrowser.h"

#include <imgui.h>
#include <filesystem>
#include <vector>
#include <string>

#include <ImGuiNotify.hpp>

namespace fs = std::filesystem;

#ifdef __EMSCRIPTEN__
static fs::path s_CurrentDirectory = "/";
#else
static fs::path s_CurrentDirectory = RESOURCE_DIR;
#endif

namespace editor
{
void ImguiContentBrowser::draw()
{
	return;
	ImGui::Begin("Content Browser");

	if (s_CurrentDirectory.has_parent_path())
	{
		if (ImGui::Button(".."))
		{
			s_CurrentDirectory = s_CurrentDirectory.parent_path();
		}
	}

	// todo: Button, Selectable -> add Icon
	for (auto& entry : fs::directory_iterator(s_CurrentDirectory))
	{
		const auto& path = entry.path();
		std::string name = path.filename().string();

		if (entry.is_directory())
		{
			if (ImGui::Button(name.c_str()))
			{
				s_CurrentDirectory /= path.filename();
			}
		}
		else
		{
			ImGui::Selectable(name.c_str());

			ImGui::PushID(name.c_str());
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", path.string().c_str(), path.string().length() + 1);
				ImGui::Text("%s", name.c_str());
				ImGui::EndDragDropSource();
			}
			ImGui::PopID();
		}
	}

	ImGui::End();
}

}	 // namespace editor