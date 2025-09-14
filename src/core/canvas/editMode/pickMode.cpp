#include "pickMode.h"

#include "../canvas.h"
#include "../animationCreatorCanvas.h"

#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "scene/ui/bbox.h"

namespace core
{

PickMode::PickMode(AnimationCreatorCanvas* canvas)
{
	rCanvas = canvas;
	onInputAttach({1.0f, 1.0f});
}

PickMode::~PickMode()
{
	if (mContext.hover.isNull() == false)
	{
		rCanvas->mControlScene->destroyEntity(mContext.hover);
	}
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
		if (mContext.bbox)
		{
			auto targetId = mContext.pickInfo.currentSelectedPaint->id;
			auto targetEntity = mContext.pickInfo.currentSelectedScene->getEntityById(targetId);
			mContext.bbox->retarget(targetEntity);
			mContext.pickInfo.excludeIds.insert(targetId);
			return true;
		}
	}
	else if (mContext.bbox)
	{
		mContext.bbox->retarget(Entity());
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
		onInputAttach({1.0f, 1.0f});
		if (mContext.bbox == nullptr)
		{
			auto bbox = rCanvas->mControlScene->createEntity("bbox");
			auto& bboxComponent = bbox.addComponent<BBoxControlComponent>();
			bboxComponent.bbox =
				std::make_unique<BBox>(rCanvas->getInputController(), rCanvas->mControlScene.get(), targetEntity);
			mContext.bbox = bboxComponent.bbox.get();
		}
		else
		{
			mContext.bbox->retarget(targetEntity);
			return mContext.bbox->onStartClickLeftMouse(inputValue);
		}
	}
	else
	{
		auto size = rCanvas->mSize;
		auto sp = mContext.startPoint;
		if (mContext.bbox && size.x > sp.x && size.y > sp.y && sp.x > 0 && sp.y > 0)
		{
			mContext.bbox->retarget(Entity());
		}
		mContext.pickInfo.currentSelectedPaint = nullptr;
		mContext.pickInfo.currentSelectedScene = nullptr;
	}
	return true;
}
bool PickMode::onDragLeftMouse(const InputValue& inputValue)
{
	if (mContext.pickInfo.currentSelectedScene && mContext.bbox)
		return mContext.bbox->onDragLeftMouse(inputValue);
	return true;
}
bool PickMode::onEndLeftMouse(const InputValue& inputValue)
{
	return true;
}
bool PickMode::onMoveMouse(const InputValue& inputValue)
{
	// todo: destroy/create -> hide/show
	if (!mContext.hover.isNull())
	{
		rCanvas->mControlScene->destroyEntity(mContext.hover);
	}

	if (mContext.bbox && mContext.bbox->onMoveMouse(inputValue))
	{
		return true;
	}
	PickInfo pickInfo;

	bool isPick = Pick(rCanvas->mMainScene->getScene(), pickInfo, inputValue.get<Vec2>());
	if (isPick && pickInfo.type == PickInfo::Type::Shape)
	{
		std::array<Vec2, 4> points = GetObb(pickInfo.currentSelectedPaint);
		mContext.hover = rCanvas->mControlScene->createObb(points);
		mContext.hover.getComponent<StrokeComponent>().color = CommonSetting::Color_DefaultHoverOutline;
	}

	return true;
}

bool PickMode::onInputAttach(const InputValue& inputValue)
{
	if (mContext.bbox != nullptr)
		return false;

	if (auto v = rCanvas->mControlScene->findByComponent<BBoxControlComponent>(); !v.empty())
	{
		mContext.bbox = v[0].getComponent<BBoxControlComponent>().bbox.get();
	}
	else
		mContext.bbox = nullptr;

	return false;
}

bool PickMode::onInputDetach(const InputValue& inputValue)
{
	mContext.isLeftMouseDown = false;
	mContext.bbox = nullptr;

	return false;
}
}	 // namespace core