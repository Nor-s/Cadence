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
		{
			canvas.moveCamera(updateSize * 0.5f);
			App::PushEvent<CanvasResizeEvent>(canvasIndex, updateSize);
		}
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

	{
		if (ImGui::CollapsingHeader("Draw Order"))
		{
			// todo: use api
			if (ImGui::Button("To Forward"))
			{
				entity.changeDrawOrder(core::ChangeOrderType::ToForward);
			}
			ImGui::SameLine();
			if (ImGui::Button("To Backward"))
			{
				entity.changeDrawOrder(core::ChangeOrderType::ToBackward);
			}
			if (ImGui::Button("To Front"))
			{
				entity.changeDrawOrder(core::ChangeOrderType::ToFront);
			}
			ImGui::SameLine();
			if (ImGui::Button("To Back"))
			{
				entity.changeDrawOrder(core::ChangeOrderType::ToBack);
			}
		}
	}

	// transform component
	{
		if (ImGui::CollapsingHeader("Transform"))
		{
			auto& transform = entity.getComponent<core::TransformComponent>();
			auto position = transform.localPosition;
			ImGui::Helper::DragFPropertyXYZ("Position:", position.value, 0.001f, -1000.0f, 1000.0f, "%.3f", "Position",
											2);

			auto scale = transform.scale;
			ImGui::Helper::DragFPropertyXYZ("Scale:", scale.value, 0.001f, -1000.0f, 1000.0f, "%.3f", "Scale", 2);

			auto rotation = transform.rotation;
			ImGui::Helper::DragFPropertyXYZ("Rotation:", &rotation, 0.001f, -1000.0f, 1000.0f, "%.3f", "Rotation", 1);
		}
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
														   isBeforeColorEdit && !isSFColorEdit, false);
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
					UpdateEntitySolidFillAlphaCurrentFrame(entity.getId(), alpha, IsBeforeAlphaEdit && !isSFAlphaEdit,
														   false);
				}

				if (ImGui::Button("- Fill"))
				{
					RemoveFillComponent(entity.getId());
				}
			}
		}
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
														IsBeforeStrokeWidthEdit && !isStrokeWidthEdit, false);
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
														isBeforeColorEdit && !isSColorEdit, false);
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
					UpdateEntityStrokeAlphaCurrentFrame(entity.getId(), alpha, IsBeforeAlphaEdit && !isSAlphaEdit,
														false);
				}

				if (ImGui::Button("- Stroke"))
				{
					RemoveStrokeComponent(entity.getId());
				}
			}
		}
	}	 // stroke component
	if (entity.hasComponent<core::PathListComponent>())
	{
		auto& path = entity.getComponent<core::PathListComponent>();
		for (int i = 0; i < path.paths.size(); i++)
		{
			ImGui::PushID(i);
			auto* p = path.paths[i].get();
			switch (p->type())
			{
				case core::IPath::Type::Ellipse:
					drawEllipseComponent(entity, *static_cast<core::EllipsePath*>(p), i);
					break;
				case core::IPath::Type::Polygon:
					drawPolygonComponent(entity, *static_cast<core::PolygonPath*>(p), i);
					break;
				case core::IPath::Type::Star:
					drawStarPolygonComponent(entity, *static_cast<core::StarPolygonPath*>(p), i);
					break;
				case core::IPath::Type::Rect:
					drawRectComponent(entity, *static_cast<core::RectPath*>(p), i);
					break;
			}
			ImGui::PopID();
		}
	}
}

void ImGuiCanvasView::drawRectComponent(core::Entity& entity, core::RectPath& path, int idx)
{
	if (ImGui::CollapsingHeader("Rect Path"))
	{
		// Radius
		static bool isRectRadiusEdit = false;
		bool wasRectRadiusEdit = isRectRadiusEdit;
		ImGui::Text("Radius:");
		ImGui::SameLine();
		auto radius = path.radius;
		isRectRadiusEdit = ImGui::DragFloat("## Rect radius", &radius, 0.1f, 0.0f, 1000.0f, "%.1f");
		if (isRectRadiusEdit || wasRectRadiusEdit)
		{
			UpdateEntityRectPathRadiusCurrentFrame(entity.getId(), idx, radius,
												   /*finished=*/wasRectRadiusEdit && !isRectRadiusEdit, false);
		}

		// Position
		static bool isRectPosEdit = false;
		bool wasRectPosEdit = isRectPosEdit;
		ImGui::Text("Position:");
		ImGui::SameLine();
		auto pos = path.position;
		isRectPosEdit = ImGui::DragFloat2("## Rect position", &pos.x, 0.1f, -10000.0f, 10000.0f, "%.1f");
		if (isRectPosEdit || wasRectPosEdit)
		{
			UpdateEntityRectPathPositionCurrentFrame(entity.getId(), idx, pos.x, pos.y,
													 /*finished=*/wasRectPosEdit && !isRectPosEdit, false);
		}

		// Scale
		static bool isRectScaleEdit = false;
		bool wasRectScaleEdit = isRectScaleEdit;
		ImGui::Text("Scale:");
		ImGui::SameLine();
		auto scl = path.scale;
		isRectScaleEdit = ImGui::DragFloat2("## Rect scale", &scl.x, 0.1f, 0.0f, 10000.0f, "%.1f");
		if (isRectScaleEdit || wasRectScaleEdit)
		{
			UpdateEntityRectPathScaleCurrentFrame(entity.getId(), idx, scl.x, scl.y,
												  /*finished=*/wasRectScaleEdit && !isRectScaleEdit, false);
		}
	}
}

void ImGuiCanvasView::drawEllipseComponent(core::Entity& entity, core::EllipsePath& path, int idx)
{
	if (ImGui::CollapsingHeader("Elipse Path"))
	{
		// Position
		static bool isEllipsePosEdit = false;
		bool wasEllipsePosEdit = isEllipsePosEdit;
		ImGui::Text("Position:");
		ImGui::SameLine();
		auto pos = path.position;
		isEllipsePosEdit = ImGui::DragFloat2("## Elipse position", &pos.x, 0.1f, -10000.0f, 10000.0f, "%.1f");
		if (isEllipsePosEdit || wasEllipsePosEdit)
		{
			UpdateEntityElipsePathPositionCurrentFrame(entity.getId(), idx, pos.x, pos.y,
													   /*finished=*/wasEllipsePosEdit && !isEllipsePosEdit, false);
		}

		// Scale
		static bool isEllipseScaleEdit = false;
		bool wasEllipseScaleEdit = isEllipseScaleEdit;
		ImGui::Text("Scale:");
		ImGui::SameLine();
		auto scl = path.scale;
		isEllipseScaleEdit = ImGui::DragFloat2("## Elipse scale", &scl.x, 0.1f, 0.0f, 10000.0f, "%.1f");
		if (isEllipseScaleEdit || wasEllipseScaleEdit)
		{
			UpdateEntityElipsePathScaleCurrentFrame(entity.getId(), idx, scl.x, scl.y,
													/*finished=*/wasEllipseScaleEdit && !isEllipseScaleEdit, false);
		}
	}
}

void ImGuiCanvasView::drawPolygonComponent(core::Entity& entity, core::PolygonPath& path, int idx)
{
	if (ImGui::CollapsingHeader("Polygon Path"))
	{
		// Points
		static bool isPolyPointsEdit = false;
		bool wasPolyPointsEdit = isPolyPointsEdit;
		ImGui::Text("Points:");
		ImGui::SameLine();
		int points = path.points;
		isPolyPointsEdit = ImGui::DragInt("## Polygon points", &points, 1, 3, 30);
		if (isPolyPointsEdit || wasPolyPointsEdit)
		{
			UpdateEntityPolygonPathPointsCurrentFrame(entity.getId(), idx, points,
													  /*finished=*/wasPolyPointsEdit && !isPolyPointsEdit, false);
		}

		// Rotation
		// static bool isPolyRotEdit = false;
		// bool wasPolyRotEdit = isPolyRotEdit;
		// ImGui::Text("Rotation:");
		// ImGui::SameLine();
		// auto rot = poly.rotation;
		// isPolyRotEdit = ImGui::DragFloat("## Polygon rotation", &rot, 0.1f, -360.0f, 360.0f, "%.1f");
		// if (isPolyRotEdit || wasPolyRotEdit)
		//{
		//	UpdateEntityPolygonPathRotationCurrentFrame(entity.getId(), rot,
		//												/*finished=*/wasPolyRotEdit && !isPolyRotEdit);
		//}

		// Outer Radius
		static bool isPolyOuterRadiusEdit = false;
		bool wasPolyOuterRadiusEdit = isPolyOuterRadiusEdit;
		ImGui::Text("Outer Radius:");
		ImGui::SameLine();
		auto r = path.outerRadius;
		isPolyOuterRadiusEdit = ImGui::DragFloat("## Polygon outer radius", &r, 0.1f, 0.0f, 10000.0f, "%.1f");
		if (isPolyOuterRadiusEdit || wasPolyOuterRadiusEdit)
		{
			UpdateEntityPolygonPathOuterRadiusCurrentFrame(
				entity.getId(), idx, r,
				/*finished=*/wasPolyOuterRadiusEdit && !isPolyOuterRadiusEdit, false);
		}

		// Position
		// static bool isPolyPosEdit = false;
		// bool wasPolyPosEdit = isPolyPosEdit;
		// ImGui::Text("Position:");
		// ImGui::SameLine();
		// auto pos = poly.position;
		// isPolyPosEdit = ImGui::DragFloat2("## Polygon position", &pos.x, 0.1f, -10000.0f, 10000.0f, "%.1f");
		// if (isPolyPosEdit || wasPolyPosEdit)
		//{
		//	UpdateEntityPolygonPathPositionCurrentFrame(entity.getId(), pos.x, pos.y,
		//												/*finished=*/wasPolyPosEdit && !isPolyPosEdit);
		//}
	}
}
void ImGuiCanvasView::drawStarPolygonComponent(core::Entity& entity, core::StarPolygonPath& path, int idx)
{
	if (ImGui::CollapsingHeader("Star Polygon Path"))
	{
		// Points
		static bool isStarPointsEdit = false;
		bool wasStarPointsEdit = isStarPointsEdit;
		ImGui::Text("Points:");
		ImGui::SameLine();
		int points = path.points;
		isStarPointsEdit = ImGui::DragInt("## Star points", &points, 1, 3, 30);
		if (isStarPointsEdit || wasStarPointsEdit)
		{
			UpdateEntityStarPolygonPathPointsCurrentFrame(entity.getId(), idx, points,
														  /*finished=*/wasStarPointsEdit && !isStarPointsEdit, false);
		}

		// Rotation
		// static bool isStarRotEdit = false;
		// bool wasStarRotEdit = isStarRotEdit;
		// ImGui::Text("Rotation:");
		// ImGui::SameLine();
		// auto rot = star.rotation;
		// isStarRotEdit = ImGui::DragFloat("## Star rotation", &rot, 0.1f, -360.0f, 360.0f, "%.1f");
		// if (isStarRotEdit || wasStarRotEdit)
		//{
		//	UpdateEntityStarPolygonPathRotationCurrentFrame(entity.getId(), rot,
		//													/*finished=*/wasStarRotEdit && !isStarRotEdit);
		//}

		// Outer Radius
		static bool isStarOuterRadiusEdit = false;
		bool wasStarOuterRadiusEdit = isStarOuterRadiusEdit;
		ImGui::Text("Outer Radius:");
		ImGui::SameLine();
		auto ro = path.outerRadius;
		isStarOuterRadiusEdit = ImGui::DragFloat("## Star outer radius", &ro, 0.1f, 0.0f, 10000.0f, "%.1f");
		if (isStarOuterRadiusEdit || wasStarOuterRadiusEdit)
		{
			UpdateEntityStarPolygonPathOuterRadiusCurrentFrame(
				entity.getId(), idx, ro,
				/*finished=*/wasStarOuterRadiusEdit && !isStarOuterRadiusEdit, false);
		}

		// Inner Radius
		static bool isStarInnerRadiusEdit = false;
		bool wasStarInnerRadiusEdit = isStarInnerRadiusEdit;
		ImGui::Text("Inner Radius:");
		ImGui::SameLine();
		auto ri = path.innerRadius;
		isStarInnerRadiusEdit = ImGui::DragFloat("## Star inner radius", &ri, 0.1f, 0.0f, 10000.0f, "%.1f");
		if (isStarInnerRadiusEdit || wasStarInnerRadiusEdit)
		{
			UpdateEntityStarPolygonPathInnerRadiusCurrentFrame(
				entity.getId(), idx, ri,
				/*finished=*/wasStarInnerRadiusEdit && !isStarInnerRadiusEdit, false);
		}

		// Position
		// static bool isStarPosEdit = false;
		// bool wasStarPosEdit = isStarPosEdit;
		// ImGui::Text("Position:");
		// ImGui::SameLine();
		// auto pos = star.position;
		// isStarPosEdit = ImGui::DragFloat2("## Star position", &pos.x, 0.1f, -10000.0f, 10000.0f, "%.1f");
		// if (isStarPosEdit || wasStarPosEdit)
		//{
		//	UpdateEntityStarPolygonPathPositionCurrentFrame(entity.getId(), pos.x, pos.y,
		//													/*finished=*/wasStarPosEdit && !isStarPosEdit);
		//}
	}
}

}	 // namespace editor