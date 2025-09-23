#include "selectionManager.h"
#include "canvas/animationCreatorCanvas.h"
#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "scene/ui/editPath.h"
#include "scene/ui/bbox.h"

#include "canvas/shapeUtil.h"
#include "canvas/animationCreatorInputController.h"

namespace core
{

void SelectionManager::Select(AnimationCreatorCanvas* canvas, Entity entity)
{
	Clear(canvas);
	Get().mSelectList[canvas].push_back(entity);
	if (canvas->mInputController->getMode() != EditModeType::PICK)
	{
		canvas->mInputController->setMode(EditModeType::PICK);
	}
}
void SelectionManager::Hover(AnimationCreatorCanvas* canvas, Entity entity)
{
	Get().mHover[canvas] = entity;
}
void SelectionManager::Push(AnimationCreatorCanvas* canvas, Entity entity)
{
	// todo: multiSelection
	Select(canvas, entity);
	// Get().mSelectList.push_back(entity);
}

void SelectionManager::Update(AnimationCreatorCanvas* canvas)
{
	bool isEditPath = Get().updateEditPath(canvas);
	Get().updateHover(canvas);
	Get().updateSelect(canvas, isEditPath);
}

bool SelectionManager::IsSelected(AnimationCreatorCanvas* canvas, int id)
{
	for (auto& entity : Get().mSelectList[canvas])
	{
		if (entity.getId() == id)
			return true;
	}
	return false;
}

bool SelectionManager::SetEditPath(AnimationCreatorCanvas* canvas, Entity entity, int pathIdx)
{
	if (canvas == nullptr || entity.isNull() || entity.isHidden())
	{
		return false;
	}
	Select(canvas, entity);
	Get().mEditPath[canvas] = pathIdx;
	return true;
}

Entity SelectionManager::GetFirstSelectedEntity(AnimationCreatorCanvas* canvas)
{
	auto& selectList = Get().mSelectList[canvas];
	if (selectList.empty())
	{
		return Entity();
	}
	return selectList.at(0);
}

void SelectionManager::Update()
{
}

void SelectionManager::Clear(AnimationCreatorCanvas* canvas)
{
	Get().mSelectList[canvas].clear();
	Get().mEditPath[canvas] = -1;
}

SelectionManager& SelectionManager::Get()
{
	static SelectionManager sSelectionMgr;
	return sSelectionMgr;
}

void SelectionManager::updateSelect(AnimationCreatorCanvas* canvas, bool disable)
{
	auto& entityList = mSelectList[canvas];
	BBox* pBbox = nullptr;
	Entity target;
	if (!disable && entityList.empty() == false)
	{
		target = entityList.front();
	}

	if (auto v = canvas->mControlScene->findByComponent<BBoxControlComponent>(); !v.empty())
	{
		pBbox = v[0].getComponent<BBoxControlComponent>().bbox.get();
		pBbox->retarget(target);
	}
	else
	{
		auto bbox = canvas->mControlScene->createEntity("bbox");
		auto& bboxComp = bbox.addComponent<BBoxControlComponent>();
		bboxComp.bbox = std::make_unique<BBox>(canvas->getInputController(), canvas->mControlScene.get(), target);
	}
}

void SelectionManager::updateHover(AnimationCreatorCanvas* canvas)
{
	static std::unordered_map<AnimationCreatorCanvas*, Entity> hover;
	if (!hover[canvas].isNull())
	{
		canvas->mControlScene->destroyEntity(hover[canvas]);
	}

	auto target = mHover[canvas];
	if (!target.isNull() && target.hasComponent<ShapeComponent>())
	{
		std::array<Vec2, 4> points = GetObb(target.getComponent<ShapeComponent>().shape);
		hover[canvas] = canvas->mControlScene->createObb(points);
		hover[canvas].getComponent<StrokeComponent>().color = CommonSetting::Color_DefaultHoverOutline;
	}
}

bool SelectionManager::updateEditPath(AnimationCreatorCanvas* canvas)
{
	auto& entities = Get().mSelectList[canvas];
	auto pathIndex = Get().mEditPath[canvas];
	EditPath* editPath = nullptr;
	Entity editPathEntity;

	if (auto v = canvas->mControlScene->findByComponent<EditPathControlComponent>(); !v.empty())
	{
		editPathEntity = v[0];
		editPath = editPathEntity.getComponent<EditPathControlComponent>().editPath.get();
	}
	if (pathIndex == -1 || entities.empty())
	{
		if (editPathEntity.isNull() == false)
		{
			canvas->mControlScene->destroyEntity(editPathEntity);
		}
		return false;
	}
	auto& targetEntity = entities.at(0);

	if (editPath == nullptr)
	{
		auto entity = canvas->mControlScene->createEntity("editPath");
		auto& editPathComponent = entity.addComponent<EditPathControlComponent>();
		editPathComponent.editPath = std::make_unique<EditPath>(canvas->getInputController(),
																canvas->mControlScene.get(), targetEntity, pathIndex);
		editPath = editPathComponent.editPath.get();
	}
	editPath->onUpdate();

	return true;
}

}	 // namespace core