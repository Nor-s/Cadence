#include "imguiShapePanel.h"

#include <core/core.h>
#include <imgui_helper.h>

#include <icons/icons.h>

namespace editor
{
struct ToolButton
{
	const char* icon;
	Edit_Mode mode;
};

void ImGuiShapePanel::draw(core::AnimationCreatorCanvas* canvas)
{
	mWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
				   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoScrollbar;

	if (GetCurrentEditMode(canvas) == Edit_Mode::EDIT_MODE_EDIT_PATH)
	{
		drawEditMode(canvas);
		return;
	}
	drawNormalMode(canvas);
}
void ImGuiShapePanel::drawNormalMode(core::AnimationCreatorCanvas* canvas)
{
	ToolButton toolButtons[] = {
		{ICON_FA_HAND, EDIT_MODE_NONE},
		{ICON_KI_CURSOR, EDIT_MODE_PICK},
		{ICON_KI_BUTTON_SQUARE, EDIT_MODE_ADD_SQUARE},
		{ICON_KI_BUTTON_CIRCLE, EDIT_MODE_ADD_ELLIPSE},
		{ICON_KI_BUTTON_TRIANGLE, EDIT_MODE_ADD_POLYGON},
		{ICON_KI_STAR, EDIT_MODE_ADD_STAR},
		{ICON_KI_PENCIL, EDIT_MODE_ADD_PEN_PATH},
	};

	ImVec2 buttonSize(40.0f, 40.0f);
	ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
	float titleBarHeight = ImGui::GetFrameHeight();

	ImVec2 windowSize =
		ImVec2(buttonSize.x + windowPadding.x * 2, buttonSize.y * 7.0f + windowPadding.y * 9 + titleBarHeight);

	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
	ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);

	auto currentMode = GetCurrentEditMode(canvas);

	if (ImGui::Begin(ICON_FA_TOOLBOX, nullptr, mWindowFlags))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[ICON_BIG]);
		for (const auto& button : toolButtons)
		{
			if (ImGui::Helper::ToggleButtonImage(button.icon, currentMode == button.mode, buttonSize))
			{
				if (currentMode != button.mode)
				{
					SetEditMode(canvas, button.mode);
				}
			}
		}

		ImGui::PopFont();
	}
	ImGui::End();
}

void ImGuiShapePanel::drawEditMode(core::AnimationCreatorCanvas* canvas)
{
	ToolButton toolButtons[] = {
		{ICON_KI_EXIT, EDIT_MODE_PICK},
	};

	ImVec2 buttonSize(40.0f, 40.0f);
	ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
	float titleBarHeight = ImGui::GetFrameHeight();

	ImVec2 windowSize =
		ImVec2(buttonSize.x + windowPadding.x * 2, buttonSize.y * 7.0f + windowPadding.y * 9 + titleBarHeight);

	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
	ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);

	auto currentMode = GetCurrentEditMode(canvas);

	if (ImGui::Begin(ICON_FA_TOOLBOX, nullptr, mWindowFlags))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[ICON_BIG]);
		for (const auto& button : toolButtons)
		{
			if (ImGui::Helper::ToggleButtonImage(button.icon, currentMode == button.mode, buttonSize))
			{
				if (currentMode != button.mode)
				{
					SetEditMode(canvas, button.mode);
				}
			}
		}

		ImGui::PopFont();
	}
	ImGui::End();
}

}	 // namespace editor