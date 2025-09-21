#include "imguiShapePanel.h"

#include <core/core.h>
#include <imgui_helper.h>

#include <icons/icons.h>

namespace editor
{

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
	struct ToolButton
	{
		const char* icon;
		core::EditModeType mode;
	};

	ToolButton toolButtons[] = {
		{ICON_FA_HAND, core::EditModeType::NONE},
		{ICON_KI_CURSOR, core::EditModeType::PICK},
		{ICON_KI_BUTTON_SQUARE, core::EditModeType::ADD_SQUARE},
		{ICON_KI_BUTTON_CIRCLE, core::EditModeType::ADD_ELLIPSE},
		{ICON_KI_BUTTON_TRIANGLE, core::EditModeType::ADD_POLYGON},
		{ICON_KI_STAR, core::EditModeType::ADD_STAR},
		{ICON_KI_PENCIL, core::EditModeType::ADD_PEN_PATH},
	};

	ImVec2 buttonSize(40.0f, 40.0f);
	ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
	float titleBarHeight = ImGui::GetFrameHeight();

	ImVec2 windowSize =
		ImVec2(buttonSize.x + windowPadding.x * 2, buttonSize.y * 7.0f + windowPadding.y * 9 + titleBarHeight);

	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
	ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);

	if (ImGui::Begin(ICON_FA_TOOLBOX, nullptr, mWindowFlags))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[ICON_BIG]);
		auto* inputController = canvas->mInputController.get();
		auto mode = inputController->getMode();
		for (const auto& button : toolButtons)
		{
			if (ImGui::Helper::ToggleButtonImage(button.icon, mode == button.mode, buttonSize))
			{
				if (mode != button.mode)
				{
					inputController->setMode(button.mode);
				}
			}
		}

		ImGui::PopFont();
	}
	ImGui::End();
}

void ImGuiShapePanel::drawEditMode(core::AnimationCreatorCanvas* canvas)
{
	struct ToolButton
	{
		const char* icon;
		core::EditModeType mode;
	};

	ToolButton toolButtons[] = {
		{ICON_KI_EXIT, core::EditModeType::PICK},
	};

	ImVec2 buttonSize(40.0f, 40.0f);
	ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;
	float titleBarHeight = ImGui::GetFrameHeight();

	ImVec2 windowSize =
		ImVec2(buttonSize.x + windowPadding.x * 2, buttonSize.y * 7.0f + windowPadding.y * 9 + titleBarHeight);

	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
	ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);

	if (ImGui::Begin(ICON_FA_TOOLBOX, nullptr, mWindowFlags))
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[ICON_BIG]);
		auto* inputController = canvas->mInputController.get();
		auto mode = inputController->getMode();
		for (const auto& button : toolButtons)
		{
			if (ImGui::Helper::ToggleButtonImage(button.icon, mode == button.mode, buttonSize))
			{
				if (mode != button.mode)
				{
					inputController->setMode(button.mode);
				}
			}
		}

		ImGui::PopFont();
	}
	ImGui::End();
}

}	 // namespace editor