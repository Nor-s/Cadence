#include "imguiTimeline.h"

#include "canvas/animationCreatorCanvas.h"
#include "animation/animator.h"

#include <core/core.h>

namespace editor
{

void ImguiTimeline::draw()
{
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

	mCurrentFrame = animator->mCurrentFrameNo;
	uint32_t start = static_cast<uint32_t>(animator->mMinFrameNo);
	uint32_t end = static_cast<uint32_t>(animator->mMaxFrameNo);
	uint32_t before = mCurrentFrame;

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

		if (ImGui::BeginNeoSequencer("Sequencer", &mCurrentFrame, &start, &end))
		{
			drawSelectedEntityKeyframe();

			mIsHoveredZoomSlider = ImGui::IsZoomSliderHovered();
			ImGui::EndNeoSequencer();
		}
	}
	ImGui::EndChild();

	// update current time
	if (before != mCurrentFrame)
	{
		animator->mCurrentFrameNo = mCurrentFrame;
		animator->mDirty = true;
	}
}

void ImguiTimeline::drawSelectedEntityKeyframe()
{
	::ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiIO& io = ImGui::GetIO();
	auto* canvas = GetCurrentAnimCanvas();
	if (canvas == nullptr)
	{
		return;
	}
	auto* animCanvas = static_cast<core::AnimationCreatorCanvas*>(canvas);
	if (animCanvas)
	{
		auto select = animCanvas->mControlScene->findByComponent<core::BBoxControlComponent>();
		if (!select.empty())
		{
			auto& bbox = select.front().getComponent<core::BBoxControlComponent>().bbox;
			auto entity = bbox->rTarget;
			drawEntityKeyframe(entity);
		}
	}
}

void ImguiTimeline::drawEntityKeyframe(core::Entity& entity)
{
	if (entity.isNull())
		return;

	drawComponents(entity);
}

void ImguiTimeline::drawComponents(core::Entity& entity)
{
	auto& name = entity.getComponent<core::NameComponent>().name;
	auto id = entity.getId();
	mDragRect = ImRect{};
	if (entity.hasComponent<core::TransformKeyframeComponent>())
	{
		auto& transformKf = entity.getComponent<core::TransformKeyframeComponent>();
		if (ImGui::BeginNeoGroup("Transform", &mEntityOpenState[(void*) &transformKf]))
		{
			drawKeyframe("Position", transformKf.positionKeyframes);
			drawKeyframe("Scale", transformKf.scaleKeyframes);
			drawKeyframe("Rotate", transformKf.rotationKeyframes);
			ImGui::EndNeoGroup();
		}
	}

	if (entity.hasComponent<core::SolidFillComponent>())
	{
		auto& fill = entity.getComponent<core::SolidFillComponent>();
		if (ImGui::BeginNeoGroup("Fill", &mEntityOpenState[(void*) &fill]))
		{
			drawKeyframe("Color", fill.colorKeyframe);
			drawKeyframe("Alpha", fill.alphaKeyframe);
			ImGui::EndNeoGroup();
		}
	}
	if (entity.hasComponent<core::StrokeComponent>())
	{
		auto& stroke = entity.getComponent<core::StrokeComponent>();
		if (ImGui::BeginNeoGroup("Stroke", &mEntityOpenState[(void*) &stroke]))
		{
			drawKeyframe("Color", stroke.colorKeyframe);
			drawKeyframe("Alpha", stroke.alphaKeyframe);
			drawKeyframe("Width", stroke.widthKeyframe);
			ImGui::EndNeoGroup();
		}
	}

	if (entity.hasComponent<core::PathListComponent>())
	{
		auto& pathList = entity.getComponent<core::PathListComponent>();
		for (size_t i = 0; i < pathList.paths.size(); ++i)
		{
			core::IPath* base = pathList.paths[i].get();
			if (!base)
				continue;

			switch (base->type())
			{
				case core::IPath::Type::Rect:
				{
					auto* p = static_cast<core::RectPath*>(base);
					std::string label = "Rect Path #" + std::to_string(i);
					if (ImGui::BeginNeoGroup(label.c_str(), &mEntityOpenState[(void*) p]))
					{
						drawKeyframe("Radius", p->radiusKeyframes);
						drawKeyframe("Position", p->positionKeyframes);
						drawKeyframe("Scale", p->scaleKeyframes);
						ImGui::EndNeoGroup();
					}
					break;
				}
				case core::IPath::Type::Ellipse:
				{
					auto* p = static_cast<core::EllipsePath*>(base);
					std::string label = "Ellipse Path #" + std::to_string(i);
					if (ImGui::BeginNeoGroup(label.c_str(), &mEntityOpenState[(void*) p]))
					{
						drawKeyframe("Position", p->positionKeyframes);
						drawKeyframe("Scale", p->scaleKeyframes);
						ImGui::EndNeoGroup();
					}
					break;
				}
				case core::IPath::Type::Polygon:
				{
					auto* p = static_cast<core::PolygonPath*>(base);
					std::string label = "Polygon Path #" + std::to_string(i);
					if (ImGui::BeginNeoGroup(label.c_str(), &mEntityOpenState[(void*) p]))
					{
						drawKeyframe("Points", p->pointsKeyframes);
						drawKeyframe("Rotation", p->rotationKeyframes);
						drawKeyframe("OuterRadius", p->outerRadiusKeyframes);
						drawKeyframe("Position", p->positionKeyframes);
						ImGui::EndNeoGroup();
					}
					break;
				}
				case core::IPath::Type::Star:
				{
					auto* p = static_cast<core::StarPolygonPath*>(base);
					std::string label = "Star Path #" + std::to_string(i);
					if (ImGui::BeginNeoGroup(label.c_str(), &mEntityOpenState[(void*) p]))
					{
						drawKeyframe("Points", p->pointsKeyframes);
						drawKeyframe("Rotation", p->rotationKeyframes);
						drawKeyframe("OuterRadius", p->outerRadiusKeyframes);
						drawKeyframe("InnerRadius", p->innerRadiusKeyframes);
						drawKeyframe("Position", p->positionKeyframes);
						ImGui::EndNeoGroup();
					}
					break;
				}
				case core::IPath::Type::Path:
				{
					/*			auto* p = static_cast<core::RawPath*>(base);
								std::string label = "Raw Path #" + std::to_string(i);
								if (ImGui::BeginNeoGroup(label.c_str(), &mEntityOpenState[(void*) p]))
								{
									ImGui::TextUnformatted("No keyframes available.");
									ImGui::EndNeoGroup();
								}*/
					break;
				}
			}
		}
	}
}

}	 // namespace editor