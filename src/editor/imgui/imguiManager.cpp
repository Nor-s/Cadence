#include "imguiManager.h"

#include "imgui.h"
#include "imgui/imguiCanvas.h"
#include "imgui/imguiShapePanel.h"
#include "imgui/imguiContentBrowser.h"
#include "imgui/imguiWindow.h"
#include "imgui/imguiSceneHierarchy.h"

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

void ImGuiManager::update()
{
}

void ImGuiManager::draw()
{
	drawDocSpace();
	ImGuiCanvasView().onDrawSceneInspect();
	ImGuiCanvasView().onDrawContentBrowser();
	ImguiContentBrowser().draw();

	for (auto& window : mWindows)
	{
		window->draw();
	}
}

void ImGuiManager::init()
{
	mWindows.push_back(std::make_unique<ImGuiSceneHierarchy>());
}

void ImGuiManager::drawDocSpace()
{
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

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

	auto& io = ImGui::GetIO();

	{
		ImGui::Begin("##ROOT DockSpace", NULL, windowFlags);

		ImGui::PopStyleVar(3);

		drawDocMenuBar();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_PassthruCentralNode);
		}
		ImGui::End();
	}
}
void ImGuiManager::drawDocMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("(TODO)New", "Ctrl+N");
			ImGui::MenuItem("(TODO)Open...", "Ctrl+O");
			ImGui::Separator();
			ImGui::MenuItem("(TODO)Save", "Ctrl+S");
			ImGui::MenuItem("(TODO)Save As GIF");
			ImGui::MenuItem("(TODO)Save As Lottie");
			ImGui::Separator();
			if (ImGui::MenuItem("(TODO)Exit"))
			{
			}
			ImGui::EndMenu();
		}
		// if (ImGui::BeginMenu("Edit"))
		//{
		//	ImGui::MenuItem("(TODO)Undo", "Ctrl+Z");
		//	ImGui::MenuItem("(TODO)Redo", "Ctrl+Y");
		//	ImGui::Separator();
		//	ImGui::MenuItem("(TODO)Cut", "Ctrl+X");
		//	ImGui::MenuItem("(TODO)Copy", "Ctrl+C");
		//	ImGui::MenuItem("(TODO)Paste", "Ctrl+V");
		//	ImGui::EndMenu();
		// }
		// if (ImGui::BeginMenu("Window"))
		//{
		//	ImGui::MenuItem("(TODO)Reset Layout");
		//	ImGui::MenuItem("(TODO)Toggle Docking");
		//	ImGui::EndMenu();
		// }
		// if (ImGui::BeginMenu("Help"))
		//{
		//	ImGui::MenuItem("(TODO)About");
		//	ImGui::EndMenu();
		// }

		ImGui::Text("    | ");
		ImGui::SameLine();

		enum class ModeType
		{
			Normal,
			Focus,
			Rig,
			State,
			Physics,
			Script,
			AI,
		};
		static ModeType currentMode = ModeType::Normal;

		auto RadioButtonLike = [&](const char* label, ModeType value)
		{
			bool selected = (currentMode == value);

			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 colHover = style.Colors[ImGuiCol_ButtonHovered];
			ImVec4 colBtn = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
			ImVec4 colActive = ImVec4(0.5f, 0.2f, 0.1f, 1.0f);

			if (selected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, colActive);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colHover);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, colActive);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, colBtn);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colHover);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, colBtn);
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			if (ImGui::Button(label))
			{
				currentMode = value;
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);
		};

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.0f, 0.0f});
		RadioButtonLike("Normal", ModeType::Normal);
		// ImGui::SameLine();
		// RadioButtonLike("Focus", ModeType::Focus);
		// ImGui::SameLine();
		// RadioButtonLike("Rig", ModeType::Rig);
		// ImGui::SameLine();
		// RadioButtonLike("State", ModeType::State);
		// ImGui::SameLine();
		// RadioButtonLike("Physics", ModeType::Physics);
		// ImGui::SameLine();
		// RadioButtonLike("Script", ModeType::Script);
		// ImGui::SameLine();
		// RadioButtonLike("AI", ModeType::AI);
		ImGui::PopStyleVar(1);

		ImGuiIO& io = ImGui::GetIO();
		char fps_buf[64];
		snprintf(fps_buf, sizeof(fps_buf), "FrameRate: %-10.0f", io.Framerate);

		const float availW = ImGui::GetContentRegionAvail().x;
		const float bufWidth = 120.0f;
		if (availW - bufWidth > 0)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availW - bufWidth));
			ImGui::Text(fps_buf);
		}

		ImGui::EndMenuBar();
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