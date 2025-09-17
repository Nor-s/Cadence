#ifndef _EDITOR_IMGUI_IMGUI_TIMELINE_H_
#define _EDITOR_IMGUI_IMGUI_TIMELINE_H_

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "imguiWindow.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <icons/icons.h>
#include <imgui_neo_sequencer.h>
#include <imgui_helper.h>

namespace core
{
class AnimationCreatorCanvas;
class Entity;
}	 // namespace core

namespace editor
{
class ImguiTimeline : public ImGuiWindow
{
public:
	ImguiTimeline() : ImGuiWindow(ICON_MD_SCHEDULE "Timeline")
	{
	}
	~ImguiTimeline() override = default;
	void draw() override;

	template <typename T, typename F>
	void drawPopupForCurvEdit(T& kf, F func)
	{
		ImGui::PushID(&kf);
		if (ImGui::BeginPopupContextItem("ContextMenu"))
		{
			func(kf);
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	template <typename T>
	void drawKeyframe(const std::string& name, T& kf)
	{
		if (ImGui::BeginNeoTimeline(name.c_str()))
		{
			drawPopupForCurvEdit(kf,
								 [currentFrame = mCurrentFrame](T& kf)
								 {
									 auto* left = kf.left(currentFrame);
									 auto* right = kf.right(currentFrame);
									 if (left && right)
									 {
										 auto& out = left->outTangent;
										 auto& in = right->inTangent;
										 float data[5]{out.x, out.y, in.x, in.y};
										 ImGui::Bezier("Edit Curve", data);
										 out.x = data[0];
										 out.y = data[1];
										 in.x = data[2];
										 in.y = data[3];
									 }
								 });

			for (auto& kf : kf)
			{
				ImGui::Keyframe(&kf.frame, mDragRect, nullptr, nullptr);
			}
			ImGui::EndNeoTimeLine();
		}
	}

	uint32_t mCurrentFrame = 0;
	ImRect mDragRect;

private:
	void drawSequencer();
	void drawSelectedEntityKeyframe();
	void drawEntityKeyframe(core::Entity& entity);
	void drawComponents(core::Entity& entity);

private:
	core::AnimationCreatorCanvas* rCanvas = nullptr;
	bool mIsHoveredZoomSlider = false;
	ImGuiWindowFlags mWindowFlags = 0;
	std::unordered_map<void*, bool> mEntityOpenState;
};

}	 // namespace editor

#endif