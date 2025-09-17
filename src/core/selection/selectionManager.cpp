#include "selectionManager.h"
#include "canvas/animationCreatorCanvas.h"
#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "scene/ui/bbox.h"

#include "canvas/shapeUtil.h"

namespace core
{

void SelectionManager::Select(AnimationCreatorCanvas* canvas, Entity entity)
{
	Clear(canvas);
	Get().mSelectList[canvas].push_back(entity);
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
	// update hover
	{
		static std::unordered_map<AnimationCreatorCanvas*, Entity> hover;
		if (!hover[canvas].isNull())
		{
			canvas->mControlScene->destroyEntity(hover[canvas]);
		}

		auto target = Get().mHover[canvas];
		if (!target.isNull() && target.hasComponent<ShapeComponent>())
		{
			std::array<Vec2, 4> points = GetObb(target.getComponent<ShapeComponent>().shape);
			hover[canvas] = canvas->mControlScene->createObb(points);
			hover[canvas].getComponent<StrokeComponent>().color = CommonSetting::Color_DefaultHoverOutline;
		}
	}

	// update select
	{
		auto& entityList = Get().mSelectList[canvas];
		BBox* pBbox = nullptr;
		Entity target;
		if (entityList.empty() == false)
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
}

void SelectionManager::Update()
{
}

void SelectionManager::Clear(AnimationCreatorCanvas* canvas)
{
	Get().mSelectList[canvas].clear();
}

SelectionManager& SelectionManager::Get()
{
	static SelectionManager sSelectionMgr;
	return sSelectionMgr;
}

}	 // namespace core