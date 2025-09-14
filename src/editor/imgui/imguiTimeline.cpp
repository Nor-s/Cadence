#include "imguiTimeline.h"

#include "canvas/animationCreatorCanvas.h"
#include "animation/animator.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_neo_sequencer.h>
#include <icons/icons.h>

#include <core/core.h>

namespace editor
{

void ImguiTimeline::draw(core::AnimationCreatorCanvas* canvas)
{
	rCanvas = canvas;

	mWindowFlags = 0;
	if (mIsHoveredZoomSlider)
	{
		mWindowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
	}

	ImGui::Begin(ICON_MD_SCHEDULE " Animation", 0, mWindowFlags);
	{
		drawSequencer();
	}
	ImGui::End();
}

void ImguiTimeline::drawSequencer()
{
	auto* canvas = GetCurrentAnimCanvas();
	if (canvas == nullptr)
	{
		return;
	}
	auto* animCanvas = static_cast<core::AnimationCreatorCanvas*>(canvas);
	auto* animator = animCanvas->mAnimator.get();

	uint32_t current = animator->mCurrentFrameNo;
	uint32_t start = static_cast<uint32_t>(animator->mMinFrameNo);
	uint32_t end = static_cast<uint32_t>(animator->mMaxFrameNo);
	uint32_t before = current;

	if (ImGui::Button("Play"))
	{
		animator->play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		animator->stop();
	}
	ImGui::BeginChild("##Timeline", ImVec2(0, 0), false, mWindowFlags);
	{
		auto win_pos = ImGui::GetWindowPos();
		auto viewportPanelSize = ImGui::GetContentRegionAvail();
		win_pos.y = ImGui::GetCursorPosY();

		if (ImGui::BeginNeoSequencer("Sequencer", &current, &start, &end))
		{
			// draw_keyframes(ui_context, anim_component->get_animation());

			mIsHoveredZoomSlider = ImGui::IsZoomSliderHovered();
			ImGui::EndNeoSequencer();
		}
	}
	ImGui::EndChild();

	// update current time
	if (before != current)
	{
		animator->mCurrentFrameNo = current;
		animator->mDirty = true;
	}
}

}	 // namespace editor