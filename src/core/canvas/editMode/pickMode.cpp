#include "pickMode.h"

#include "../canvas.h"
#include "../animationCreatorCanvas.h"

#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "scene/ui/bbox.h"

#include "selection/selectionManager.h"

namespace core
{

PickMode::PickMode(AnimationCreatorCanvas* canvas)
{
	rCanvas = canvas;
}

PickMode::~PickMode()
{
}

void PickMode::onUpdate()
{
}

bool PickMode::onDoubleClickLeftMouse(const InputValue& inputValue)
{
	auto pos = inputValue.get<Vec2>();
	mContext.pickInfo.currentSelectedPaint = nullptr;
	bool isPick = Pick(rCanvas->mMainScene->getScene(), mContext.pickInfo, pos);
	bool isNeedReset = false;
	if (isPick && mContext.pickInfo.type == PickInfo::Type::Shape)
	{
		auto targetId = mContext.pickInfo.currentSelectedPaint->id;
		auto targetEntity = mContext.pickInfo.currentSelectedScene->getEntityById(targetId);
		SelectionManager::Select(rCanvas, targetEntity);
		mContext.pickInfo.excludeIds.insert(targetId);
		return true;
	}
	else
	{
		SelectionManager::Clear(rCanvas);
	}

	// reset pickInfo
	{
		mContext.pickInfo.currentSelectedPaint = nullptr;
		mContext.pickInfo.currentSelectedScene = nullptr;
		mContext.pickInfo.excludeIds.clear();
	}

	return false;
}

bool PickMode::onStarClickLefttMouse(const InputValue& inputValue)
{
	mContext.isLeftMouseDown = true;
	mContext.startPoint = inputValue.get<Vec2>();
	mContext.beforePoint = mContext.startPoint;

	bool isPick = Pick(rCanvas->mMainScene->getScene(), mContext.pickInfo, mContext.startPoint);

	if (isPick && mContext.pickInfo.type == PickInfo::Type::Shape)
	{
		auto targetEntity =
			mContext.pickInfo.currentSelectedScene->getEntityById(mContext.pickInfo.currentSelectedPaint->id);
		SelectionManager::Select(rCanvas, targetEntity);
	}
	else
	{
		auto size = rCanvas->mSize;
		auto sp = mContext.startPoint;
		if (size.x > sp.x && size.y > sp.y && sp.x > 0 && sp.y > 0)
		{
			SelectionManager::Clear(rCanvas);
		}
		mContext.pickInfo.currentSelectedPaint = nullptr;
		mContext.pickInfo.currentSelectedScene = nullptr;
	}
	return true;
}
bool PickMode::onDragLeftMouse(const InputValue& inputValue)
{
	return true;
}
bool PickMode::onEndLeftMouse(const InputValue& inputValue)
{
	return true;
}
bool PickMode::onMoveMouse(const InputValue& inputValue)
{
	PickInfo pickInfo;
	bool isPick = Pick(rCanvas->mMainScene->getScene(), pickInfo, inputValue.get<Vec2>());
	if (isPick && pickInfo.type == PickInfo::Type::Shape)
	{
		auto target = pickInfo.currentSelectedScene->getEntityById(pickInfo.currentSelectedPaint->id);
		SelectionManager::Hover(rCanvas, target);
	}
	else
	{
		SelectionManager::Hover(rCanvas, Entity());
	}

	return true;
}

bool PickMode::onInputAttach(const InputValue& inputValue)
{
	return false;
}

bool PickMode::onInputDetach(const InputValue& inputValue)
{
	mContext.isLeftMouseDown = false;
	return false;
}
}	 // namespace core