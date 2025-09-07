#define IMGUI_DEFINE_MATH_OPERATORS

#include "../app.h"
#include "event/events.h"

#include "imguiCanvas.h"
#include "imguiTimeline.h"
#include "imguiShapePanel.h"
#include "examples.h"

#include <core/core.h>

#include <ImGuiNotify.hpp>
#include <imgInspect.h>
#include <imgui_internal.h>
#include <imgui_helper.h>

#include <filesystem>

namespace editor
{
core::CanvasWrapper* ImGuiCanvasView::gCurrentCanvas = nullptr;

void ImGuiCanvasView::onDraw(std::string_view title, core::CanvasWrapper& canvas, int canvasIndex)
{
	static ImGuiWindowFlags windowFlags = 0;

	auto& io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});

	if (ImGui::Begin(title.data(), 0, windowFlags | ImGuiWindowFlags_NoScrollbar))
	{
		bool isDraggingTitle = ImGui::IsItemHovered() && ImGui::IsItemClicked();
		bool isMoving = ImGui::GetCurrentContext()->MovingWindow == ImGui::GetCurrentWindow();
		ImVec2 canvasSize = ImGui::GetContentRegionAvail();
		auto textureSize = ImVec2(canvas.mSize.x, canvas.mSize.y);
		ImGui::ImageWithBg(canvas.getTexture(), textureSize, ImVec2{0, 1}, ImVec2{1, 0});

		if (gCurrentCanvas == nullptr || ImGui::IsWindowFocused())
		{
			gCurrentCanvas = &canvas;
		}

		auto rc = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		bool isMouseHoveringRect = rc.Contains({io.MousePos, io.MousePos});
		auto mousePosition = io.MousePos - rc.Min;
		auto mouseUVCoord = mousePosition / rc.GetSize();
		mouseUVCoord.y = 1.f - mouseUVCoord.y;

		// set mouseOffset for fit canvas
		if (ImGui::IsWindowFocused() && isMouseHoveringRect)
		{
			core::io::mouseOffset = {-rc.Min.x, -rc.Min.y};
		}

		// draw texture inspector
		if (isMouseHoveringRect && io.KeyShift && mouseUVCoord.x >= 0.f && mouseUVCoord.y >= 0.f)
		{
			ImageInspect::inspect(canvas.mSize.x, canvas.mSize.y, canvas.getBuffer(), mouseUVCoord, textureSize);
		}

		// import
		// todo: import event
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				auto* path = (const char*) payload->Data;
				auto ext = std::filesystem::path(path).extension();
				ImGui::InsertNotification({ImGuiToastType::Info, 3000, "File: %s", path});
				if (ext.string() == ".json")
				{
					auto [animWrap, pictureWrap] = core::AnimationWrapper::Gen(path);
					canvas.pushPaint(std::move(pictureWrap));
					canvas.pushAnimation(std::move(animWrap));
				}
				if (ext.string() == ".png" || ext.string() == ".svg" || ext.string() == ".jpg" ||
					ext.string() == ".webp")
				{
					canvas.pushPaint(core::PictureWrapper::Gen(path));
				}
			}
			ImGui::EndDragDropTarget();
		}

		// todo: push when change focus, resize
		auto updateSize = core::Size{max(1.0f, canvasSize.x), max(1.0f, canvasSize.y)};
		bool needResize = canvasSize.x != textureSize.x || canvasSize.y != textureSize.y;
		App::PushEvent<CanvasFocusEvent>(canvasIndex,
										 ImGui::IsWindowFocused() && !isDraggingTitle && !needResize && !isMoving);
		if (needResize)
			App::PushEvent<CanvasResizeEvent>(canvasIndex, updateSize);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void ImGuiCanvasView::onDrawSceneInspect()
{
	if (gCurrentCanvas == nullptr || gCurrentCanvas->type() != core::CanvasType::LottieCreator)
		return;

	auto* lottieCanvas = static_cast<core::AnimationCreatorCanvas*>(gCurrentCanvas);
	ImguiTimeline().draw(lottieCanvas);
	ImGuiShapePanel().draw(lottieCanvas);

	if (ImGui::Begin("Property", 0, 0))
	{
		drawExampleCanvasContent();
		drawAnimationCanvasProperties();
	}
	ImGui::End();
}

void ImGuiCanvasView::onDrawContentBrowser()
{
}

void ImGuiCanvasView::drawExampleCanvasContent()
{
	if (gCurrentCanvas != nullptr)
	{
		if (gCurrentCanvas && gCurrentCanvas->type() == core::CanvasType::Example)
		{
			auto* exampleCanvas = static_cast<tvgexam::ExampleCanvas*>(gCurrentCanvas);

			int currentExampleIndex = exampleCanvas->mCurrentExampleIdx;
			int beforeExampleIdx = currentExampleIndex;
			ImGui::Combo(
				"examples", &currentExampleIndex,
				[](void* data, int n) { return tvgexam::ExampleCanvas::gExampleList[n]->toString().data(); }, nullptr,
				tvgexam::ExampleCanvas::gExampleList.size());

			if (beforeExampleIdx != currentExampleIndex)
			{
				exampleCanvas->mCurrentExampleIdx = currentExampleIndex;
			}
			if (ImGui::Button("Clear"))
			{
				exampleCanvas->onInit();
			}
			ImGui::Separator();
		}
	}
}

void ImGuiCanvasView::drawAnimationCanvasProperties()
{
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

			drawComponent(entity);
		}
	}
}
void ImGuiCanvasView::drawComponent(core::Entity& entity)
{
	if (entity.isNull() || entity.hasComponent<core::SceneComponent>())
	{
		return;
	}

	// transform component
	{
		if (ImGui::CollapsingHeader("Transform"))
		{
			auto& transform = entity.getComponent<core::TransformComponent>();
			auto position = transform.localCenterPosition;
			ImGui::Helper::DragFPropertyXYZ("Position:", position.value, 0.001f, -1000.0f, 1000.0f, "%.3f", "Position",
											2);

			auto scale = transform.scale;
			ImGui::Helper::DragFPropertyXYZ("Scale:", scale.value, 0.001f, -1000.0f, 1000.0f, "%.3f", "Scale", 2);

			auto rotation = transform.rotation;
			ImGui::Helper::DragFPropertyXYZ("Rotation:", &rotation, 0.001f, -1000.0f, 1000.0f, "%.3f", "Rotation", 1);
		}
		ImGui::Separator();
	}

	// solid fill component
	{
		if (ImGui::CollapsingHeader("Fill"))
		{
			if (!entity.hasComponent<core::SolidFillComponent>())
			{
				if (ImGui::Button("+ Fill"))
				{
					AddFillComponent(entity.getId());
				}
			}
			else
			{
				static bool isSFColorEdit = false;
				bool isBeforeColorEdit = isSFColorEdit;
				auto fill = entity.getComponent<core::SolidFillComponent>();
				auto color = fill.color / 255.0f;
				ImGui::Text("Color: ");
				ImGui::SameLine();
				isSFColorEdit = ImGui::ColorEdit3("## Solid fill color", color.value);
				if (isSFColorEdit || isBeforeColorEdit)
				{
					color = color * 255.0f;
					UpdateEntitySolidFillColorCurrentFrame(entity.getId(), color.r, color.g, color.b,
														   isBeforeColorEdit && !isSFColorEdit);
				}

				static bool isSFAlphaEdit = false;
				bool IsBeforeAlphaEdit = isSFAlphaEdit;
				ImGui::Text("Alpha: ");
				ImGui::SameLine();
				auto alpha = fill.alpha / 255.0f;
				isSFAlphaEdit = ImGui::DragFloat("## Solid fill alpha", &alpha, 0.001f, 0.0f, 1.0f);
				if (isSFAlphaEdit || IsBeforeAlphaEdit)
				{
					alpha = alpha * 255.0f;
					UpdateEntitySolidFillAlphaCurrentFrame(entity.getId(), alpha, IsBeforeAlphaEdit && !isSFAlphaEdit);
				}

				if (ImGui::Button("- Fill"))
				{
					RemoveFillComponent(entity.getId());
				}
			}
		}
		ImGui::Separator();
	}	 // solid fill component

	// stroke component
	{
		if (ImGui::CollapsingHeader("Stroke"))
		{
			if (!entity.hasComponent<core::StrokeComponent>())
			{
				if (ImGui::Button("+ Stroke"))
				{
					AddStrokeComponent(entity.getId());
				}
			}
			else
			{
				auto& stroke = entity.getComponent<core::StrokeComponent>();

				static bool isStrokeWidthEdit = false;
				bool IsBeforeStrokeWidthEdit = isStrokeWidthEdit;
				ImGui::Text("Width: ");
				ImGui::SameLine();
				auto width = stroke.width;
				isStrokeWidthEdit = ImGui::DragFloat("## Stroke width", &width, 0.1f, 0.0f, 50.0f);
				if (isStrokeWidthEdit || IsBeforeStrokeWidthEdit)
				{
					UpdateEntityStrokeWidthCurrentFrame(entity.getId(), width,
														IsBeforeStrokeWidthEdit && !isStrokeWidthEdit);
				}

				static bool isSColorEdit = false;
				bool isBeforeColorEdit = isSColorEdit;
				auto color = stroke.color / 255.0f;
				ImGui::Text("Color: ");
				ImGui::SameLine();
				isSColorEdit = ImGui::ColorEdit3("## Stroke color", color.value);
				if (isSColorEdit || isBeforeColorEdit)
				{
					color = color * 255.0f;
					UpdateEntityStrokeColorCurrentFrame(entity.getId(), color.r, color.g, color.b,
														isBeforeColorEdit && !isSColorEdit);
				}

				static bool isSAlphaEdit = false;
				bool IsBeforeAlphaEdit = isSAlphaEdit;
				ImGui::Text("Alpha: ");
				ImGui::SameLine();
				auto alpha = stroke.alpha / 255.0f;
				isSAlphaEdit = ImGui::DragFloat("## Stroke alpha", &alpha, 0.001f, 0.0f, 1.0f);
				if (isSAlphaEdit || IsBeforeAlphaEdit)
				{
					alpha = alpha * 255.0f;
					UpdateEntityStrokeAlphaCurrentFrame(entity.getId(), alpha, IsBeforeAlphaEdit && !isSAlphaEdit);
				}

				if (ImGui::Button("- Stroke"))
				{
					RemoveStrokeComponent(entity.getId());
				}
			}
		}
		ImGui::Separator();
	}	 // stroke component
}

}	 // namespace editor