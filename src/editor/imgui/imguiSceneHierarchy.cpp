#include "imguiSceneHierarchy.h"
#include <core/core.h>
#include <icons/icons.h>

namespace editor
{
static void DrawLeafLine(const char* label)
{
	ImGui::TextUnformatted(ICON_MD_TUNE);
	ImGui::SameLine();
	ImGui::TextUnformatted(label);
}

static std::string GetEntityName(const core::Entity& entity)
{
	if (auto* n = entity.tryGetComponent<core::NameComponent>())
	{
		return n->name;
	}
	return "unnamed";
}

void ImGuiSceneHierarchy::content()
{
	auto* canvas = GetCurrentAnimCanvas();
	if (canvas == nullptr)
	{
		return;
	}

	auto* animCanvas = static_cast<core::AnimationCreatorCanvas*>(canvas);
	auto* scene = animCanvas->mMainScene.get();
	const auto& drawOrder = scene->getDrawOrder();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	for (const auto& item : drawOrder)
	{
		drawTreeNode(item);
	}
	ImGui::PopStyleVar();
}

void ImGuiSceneHierarchy::drawTreeNode(const core::Entity& entity)
{
	auto name = GetEntityName(entity);

	int id = entity.tryGetComponent<core::IDComponent>()->id;
	const bool hasShape = entity.hasComponent<core::ShapeComponent>();
	const std::string label = name + std::string("##") + std::to_string(id);

	if (!hasShape)
	{
		DrawLeafLine(label.c_str());
		return;
	}
	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	constexpr const char* kPayloadPathItem = "CORE_PATH_ITEM";
	struct PathDragPacket
	{
		core::EntityID srcEntityId;
		int srcIndex = -1;
	};
	bool click = ImGui::TreeNodeEx(label.c_str(), flags);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kPayloadPathItem))
		{
			const auto* pkt = static_cast<const PathDragPacket*>(payload->Data);
			MovePath((ENTITY_ID) pkt->srcEntityId, pkt->srcIndex, (ENTITY_ID) entity.getId());
		}
		ImGui::EndDragDropTarget();
	}
	if (click)
	{
		core::CallIfHas<core::SolidFillComponent>(entity, DrawLeafLine, "Fill");
		core::CallIfHas<core::StrokeComponent>(entity, DrawLeafLine, "Stroke");
		DrawLeafLine((std::string("id: ") + std::to_string(id)).c_str());

		if (entity.hasComponent<core::PathListComponent>())
		{
			auto& pathComp = entity.getComponent<core::PathListComponent>();
			auto& pathList = pathComp.paths;
			for (int i = 0; i < static_cast<int>(pathList.size()); ++i)
			{
				const char* itemLabel = nullptr;
				switch (pathList[i]->type())
				{
					case core::IPath::Type::Ellipse:
						itemLabel = "Ellipse";
						break;
					case core::IPath::Type::Rect:
						itemLabel = "Rect";
						break;
					case core::IPath::Type::Polygon:
						itemLabel = "Polygon";
						break;
					case core::IPath::Type::Star:
						itemLabel = "Star";
						break;
					case core::IPath::Type::Path:
						itemLabel = "Path";
						break;
				}

				ImGui::PushID(i);
				ImGui::Selectable(itemLabel, false, ImGuiSelectableFlags_SpanAvailWidth);

				// === Drag Source ===
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover))
				{
					PathDragPacket packet{entity.getId(), i};
					ImGui::SetDragDropPayload(kPayloadPathItem, &packet, sizeof(packet));
					ImGui::TextUnformatted(itemLabel);
					ImGui::EndDragDropSource();
				}

				ImGui::PopID();
			}

			ImGui::TreePop();
		}
	}
}

}	 // namespace editor