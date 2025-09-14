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

	if (ImGui::TreeNodeEx(label.c_str(), flags))
	{
		core::CallIfHas<core::SolidFillComponent>(entity, DrawLeafLine, "Fill");
		core::CallIfHas<core::StrokeComponent>(entity, DrawLeafLine, "Stroke");
		DrawLeafLine((std::string("id: ") + std::to_string(id)).c_str());

		if (entity.hasComponent<core::PathListComponent>())
		{
			auto& pathList = entity.getComponent<core::PathListComponent>().paths;
			for (size_t i = 0; i < pathList.size(); i++)
			{
				switch (pathList[i]->type())
				{
					case core::IPath::Type::Ellipse:
						DrawLeafLine("Ellipse");
						break;
					case core::IPath::Type::Rect:
						DrawLeafLine("Rect");
						break;
					case core::IPath::Type::Polygon:
						DrawLeafLine("Polygon");
						break;
					case core::IPath::Type::Star:
						DrawLeafLine("Star");
						break;
					case core::IPath::Type::Path:
						DrawLeafLine("Path");
						break;
				}
			}
			ImGui::TreePop();
		}
	}
}

}	 // namespace editor