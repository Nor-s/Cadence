#include "addMode.h"
#include "../canvas.h"
#include "../animationCreatorCanvas.h"
#include "scene/scene.h"

#include "interface/editInterface.h"

namespace core
{
AddMode::AddMode(AnimationCreatorCanvas* canvas, EditModeType type)
{
	mType = type;
	rCanvas = canvas;
}
AddMode::~AddMode()
{
}
void AddMode::onUpdate()
{
}
bool AddMode::onStarClickLefttMouse(const InputValue& inputValue)
{
	mContext.startPoint = inputValue.get<Vec2>();
	return true;
}
bool AddMode::onDragLeftMouse(const InputValue& inputValue)
{
	auto endPoint = inputValue.get<Vec2>();
	auto startPoint = mContext.startPoint;
	LOG_INFO("onDragLeftMouse: start {} {}, end {} {}", startPoint.x, startPoint.y, endPoint.x, endPoint.y);
	auto start = Vec2{std::min(startPoint.x, endPoint.x), std::min(startPoint.y, endPoint.y)};
	auto end = Vec2{std::max(startPoint.x, endPoint.x), std::max(startPoint.y, endPoint.y)};

	if (mContext.newEntity.mHandle != entt::null)
		rCanvas->mControlScene->destroyEntity(mContext.newEntity);

	switch (mType)
	{
		case EditModeType::ADD_SQUARE:
			mContext.newEntity = rCanvas->mControlScene->createRectFillLayer(start, end - start);
			break;
		case EditModeType::ADD_ELLIPSE:
			mContext.newEntity = rCanvas->mControlScene->createEllipseFillLayer(start, end - start);
			break;
		case EditModeType::ADD_POLYGON:
			mContext.newEntity = rCanvas->mControlScene->createPolygonFillLayer(start, end - start);
			break;
		default:
			mContext.newEntity = rCanvas->mControlScene->createStarFillLayer(start, end - start);
			break;
	};

	return true;
}
bool AddMode::onEndLeftMouse(const InputValue& inputValue)
{
	auto endPoint = inputValue.get<Vec2>();
	auto startPoint = mContext.startPoint;
	auto start = Vec2{std::min(startPoint.x, endPoint.x), std::min(startPoint.y, endPoint.y)};
	auto end = Vec2{std::max(startPoint.x, endPoint.x), std::max(startPoint.y, endPoint.y)};

	if (mContext.newEntity.mHandle != entt::null)
	{
		rCanvas->mControlScene->destroyEntity(mContext.newEntity);
	}
	auto wh = end - start;
	float threshold = 10.0f;
	if (wh.w > threshold && wh.h > threshold)
	{
		switch (mType)
		{
			case EditModeType::ADD_SQUARE:
				CreateRectPathEntity(rCanvas->mScene->mId, start.x, start.y, wh.w, wh.h);
				break;
			case EditModeType::ADD_ELLIPSE:
				CreateElipsePathEntity(rCanvas->mScene->mId, start.x, start.y, wh.w, wh.h);
				break;
			case EditModeType::ADD_POLYGON:
				CreatePolygonPathEntity(rCanvas->mScene->mId, start.x, start.y, wh.w, wh.h);
				break;
			default:
				CreateStarPathEntity(rCanvas->mScene->mId, start.x, start.y, wh.x, wh.h);
				break;
		};
	}
	else
		return false;
	return true;
}
bool AddMode::onInputDetach(const InputValue& inputValue)
{
	return false;
}

}	 // namespace core