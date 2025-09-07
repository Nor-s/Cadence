#include "imguiManager.h"

#include "imgui.h"
#include "imgui/imguiCanvas.h"
#include "imgui/imguiShapePanel.h"
#include "imgui/imguiContentBrowser.h"

#include <string>
#include <core/core.h>
#include <ImGuiNotify.hpp>
#include <icons/icons.h>

namespace editor
{

ImGuiManager::ImGuiManager()
{
	init();
	auto& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = false;
	static const ImWchar ranges[] = {0x0020, 0xFFFF, 0};
	io.Fonts->AddFontFromFileTTF(RESOURCE_DIR "/font/Pretendard-Medium.ttf", 16.0f, &config, ranges);

	ImGui::LoadInternalIcons(io.Fonts);
}

ImGuiManager::~ImGuiManager()
{
}

void ImGuiManager::draw()
{
	drawDocSpace();
	ImGuiCanvasView().onDrawSceneInspect();
	ImGuiCanvasView().onDrawContentBrowser();
	ImguiContentBrowser().draw();
}

void ImGuiManager::init()
{
}

void ImGuiManager::drawDocSpace()
{
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
					   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGui::Begin("##ROOT DockSpace", NULL, windowFlags);

		ImGui::PopStyleVar(3);

		float ribbon_h = 64;
		if (ImGui::BeginMenuBar())
		{
			ImGui::Text("🚀");
			ImGui::EndMenuBar();
		}
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_PassthruCentralNode);
		}
		ImGui::End();
	}
}
void ImGuiManager::drawCanvas(std::vector<core::CanvasWrapper*>& canvasList)
{
	for (int i = 0; i < canvasList.size(); i++)
	{
		auto& canvas = canvasList[i];
		std::string title = "canvas" + std::to_string(i);
		title += canvas->isSw() ? "(sw)" : "(gl)";
		ImGuiCanvasView().onDraw(title.c_str(), *canvas, i);
	}
}

void ImGuiManager::drawend()
{
	// Notifications style setup
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);							 // Disable round borders
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);						 // Disable borders
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f));	 // Background color
	ImGui::RenderNotifications();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(1);
}

}	 // namespace editor