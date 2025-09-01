#include "bbox.h"
#include "input/inputController.h"

#include "scene/scene.h"
#include "scene/component/components.h"

#include "canvas/shapeUtil.h"
#include "controlBox.h"

#include "common/common.h"

#include "interface/editInterface.h"

namespace core
{

BBox::BBox(InputController* inputController, core::Scene* scene, Entity target) : rScene(scene)
{
	rInputController = inputController;
	// 2 is the priority
	mInputActionBindings.push_back(inputController->bindAction(
		InputAction(InputType::MOUSE_LEFT_DOWN, 2, true), InputTrigger::Started, this, &BBox::onStartClickLeftMouse));
	mInputActionBindings.push_back(inputController->bindAction(InputAction(InputType::MOUSE_LEFT_DOWN, 2, true),
															   InputTrigger::Triggered, this, &BBox::onDragLeftMouse));
	mInputActionBindings.push_back(inputController->bindAction(InputAction(InputType::MOUSE_LEFT_DOWN, 2, true),
															   InputTrigger::Ended, this, &BBox::onEndLeftMouse));
	mInputActionBindings.push_back(inputController->bindAction(InputAction(InputType::MOUSE_MOVE, 2, true),
															   InputTrigger::Ended, this, &BBox::onMoveMouse));
	retarget(target);
}

BBox::~BBox()
{
	for (auto& binding : mInputActionBindings)
	{
		rInputController->unbinding(binding);
	}
}

// must update after update target entity
void BBox::onUpdate()
{
	if (rTarget.isNull() || !rTarget.hasComponent<ShapeComponent>())
	{
		return;
	}

	retarget(rTarget);
}

void BBox::retarget(Entity target)
{
	std::shared_ptr<BBox> bboxptr;

	if (target.isNull() || !target.hasComponent<ShapeComponent>())
	{
		rTarget = target;
		for (auto& controlBox : mControlBox)
		{
			controlBox.reset();
		}
		mCurrentControlType = ControlTypeCount;
		return;
	}

	rTarget = target;

	auto& targetShape = target.getComponent<ShapeComponent>();
	auto moveAnchorPoint = [this]()
	{
		LOG_INFO("TODO: move AnchorPoint");
		return true;
	};
	auto moveBox = [this]()
	{
		auto diff = mCurrentPoint - mBeforePoint;

		UpdateEntityDeltaPositionCurrentFrame(rTarget.getId(), diff.x, diff.y, false);

		return true;
	};
	struct ScaleFunc
	{
		static Vec2 GetRatio(const Vec2 start, const Vec2 current, tvg::Matrix toLocal)
		{
			const auto localStart = start * toLocal;

			if (std::abs(localStart.x) < 1e-6 || std::abs(localStart.y) < 1e-6)
				return {1.0f, 1.0f};
			const auto localCurrent = current * toLocal;
			return (localCurrent / localStart);
		}
	};
	auto scaleLambda = [this]()
	{
		auto ratio = ScaleFunc::GetRatio(mStartPoint, mCurrentPoint, mBeforeTransform.inverse());
		const auto currentScale = Vec2{mBeforeTransform.scale.x * ratio.x, mBeforeTransform.scale.y * ratio.y};
		UpdateEntityScaleCurrentFrame(rTarget.getId(), currentScale.x, currentScale.y, false);
		return true;
	};
	auto scaleXLambda = [this]()
	{
		auto ratio = ScaleFunc::GetRatio(mStartPoint, mCurrentPoint, mBeforeTransform.inverse());
		ratio.y = 1.0f;
		const auto currentScale = Vec2{mBeforeTransform.scale.x * ratio.x, mBeforeTransform.scale.y * ratio.y};
		UpdateEntityScaleCurrentFrame(rTarget.getId(), currentScale.x, currentScale.y, false);
		return true;
	};
	auto scaleYLambda = [this]()
	{
		auto ratio = ScaleFunc::GetRatio(mStartPoint, mCurrentPoint, mBeforeTransform.inverse());
		ratio.x = 1.0f;
		const auto currentScale = Vec2{mBeforeTransform.scale.x * ratio.x, mBeforeTransform.scale.y * ratio.y};
		UpdateEntityScaleCurrentFrame(rTarget.getId(), currentScale.x, currentScale.y, false);
		return true;
	};

	auto rotationLambda = [this]()
	{
		const auto pivot = mBeforeTransform.worldPosition;
		auto before = mBeforePoint - pivot;
		auto current = mCurrentPoint - pivot;

		const auto beforeLen = length(before);
		const auto currentLen = length(current);
		if (beforeLen < 1e-6f || currentLen < 1e-6f)
			return true;

		before = normalize(before);
		current = normalize(current);

		const auto dot = before * current;
		const auto cross = core::cross(before, current);
		const auto rad = std::atan2(cross, dot);
		const auto diff = ToDegree(rad);

		UpdateEntityDeltaRotationCurrentFrame(rTarget.getId(), diff, false);

		return true;
	};
	// todo: scale mode, dimension mode
	// todo: reset -> hide & show & move
	// todo: reset unique_ptr -> move point, change transform box (apply target transform)
	std::array<Vec2, 4> points = GetObb(targetShape.shape);
	auto& targetTransform = rTarget.getComponent<TransformComponent>();
	const auto centerPoint = targetTransform.worldPosition;
	auto wh = Vec2{CommonSetting::Width_DefaultBBoxControlBox, CommonSetting::Width_DefaultBBoxControlBox};

	mControlBox[AnchorPoint] = std::make_unique<ControlOverlay>(rScene, centerPoint, wh, ControlOverlay::Type::Move,
																ControlOverlay::ShapeType::StrokeEllipse);

	mControlBox[BoxArea] = std::make_unique<ControlOverlay>(rScene, points, ControlOverlay::Type::Move);
	mControlBox[BoxArea]->setOnLeftDrag(MakeLambda(moveBox));

	mControlBox[AnchorPoint]->setOnLeftDrag(MakeLambda(moveAnchorPoint));
	mControlBox[TopLeftScale] = std::make_unique<ControlOverlay>(rScene, points[0], wh, ControlOverlay::Type::Scale);
	mControlBox[TopLeftScale]->setOnLeftDrag(MakeLambda(scaleLambda));
	mControlBox[TopRightScale] = std::make_unique<ControlOverlay>(rScene, points[1], wh, ControlOverlay::Type::Scale);
	mControlBox[TopRightScale]->setOnLeftDrag(MakeLambda(scaleLambda));

	mControlBox[BottomLeftScale] = std::make_unique<ControlOverlay>(rScene, points[2], wh, ControlOverlay::Type::Scale);
	mControlBox[BottomLeftScale]->setOnLeftDrag(MakeLambda(scaleLambda));
	mControlBox[BottomRightScale] =
		std::make_unique<ControlOverlay>(rScene, points[3], wh, ControlOverlay::Type::Scale);
	mControlBox[BottomRightScale]->setOnLeftDrag(MakeLambda(scaleLambda));

	mControlBox[TopCenterScale] =
		std::make_unique<ControlOverlay>(rScene, (points[2] + points[3]) * 0.5f, wh, ControlOverlay::Type::Scale);
	mControlBox[TopCenterScale]->setOnLeftDrag(MakeLambda(scaleYLambda));

	mControlBox[LeftCenterScale] =
		std::make_unique<ControlOverlay>(rScene, (points[1] + points[2]) * 0.5f, wh, ControlOverlay::Type::Scale);
	mControlBox[LeftCenterScale]->setOnLeftDrag(MakeLambda(scaleXLambda));

	mControlBox[RightCenterScale] =
		std::make_unique<ControlOverlay>(rScene, (points[3] + points[0]) * 0.5f, wh, ControlOverlay::Type::Scale);
	mControlBox[RightCenterScale]->setOnLeftDrag(MakeLambda(scaleXLambda));

	mControlBox[BottomCenterScale] =
		std::make_unique<ControlOverlay>(rScene, (points[1] + points[0]) * 0.5f, wh, ControlOverlay::Type::Scale);
	mControlBox[BottomCenterScale]->setOnLeftDrag(MakeLambda(scaleYLambda));

	wh = Vec2{CommonSetting::Width_DefaultBBoxRotationControlBox, CommonSetting::Width_DefaultBBoxRotationControlBox};
	mControlBox[TopLeftRotate] = std::make_unique<ControlOverlay>(rScene, points[0], wh, ControlOverlay::Type::Rotate,
																  ControlOverlay::ShapeType::TransparentEllipse);
	mControlBox[TopLeftRotate]->setOnLeftDrag(MakeLambda(rotationLambda));
	mControlBox[TopRightRotate] = std::make_unique<ControlOverlay>(rScene, points[1], wh, ControlOverlay::Type::Rotate,
																   ControlOverlay::ShapeType::TransparentEllipse);
	mControlBox[TopRightRotate]->setOnLeftDrag(MakeLambda(rotationLambda));
	mControlBox[BottomLeftRotate] = std::make_unique<ControlOverlay>(
		rScene, points[2], wh, ControlOverlay::Type::Rotate, ControlOverlay::ShapeType::TransparentEllipse);
	mControlBox[BottomLeftRotate]->setOnLeftDrag(MakeLambda(rotationLambda));
	mControlBox[BottomRightRotate] = std::make_unique<ControlOverlay>(
		rScene, points[3], wh, ControlOverlay::Type::Rotate, ControlOverlay::ShapeType::TransparentEllipse);
	mControlBox[BottomRightRotate]->setOnLeftDrag(MakeLambda(rotationLambda));
}

bool BBox::onStartClickLeftMouse(const InputValue& inputValue)
{
	mStartPoint = inputValue.get<Vec2>();
	mCurrentPoint = mBeforePoint = mStartPoint;

	if (mCurrentControlType != ControlTypeCount)
	{
		mCurrentControlType = ControlTypeCount;
		return false;
	}

	if (rTarget.isNull())
	{
		return false;
	}

	for (int type = 0; type < ControlTypeCount; type++)
	{
		if (mControlBox[type]->onLeftDown(mStartPoint))
		{
			mBeforeTransform = rTarget.getComponent<TransformComponent>();
			mCurrentControlType = ControlType(type);
			return true;
		}
	}
	mCurrentControlType = ControlTypeCount;
	return false;
}
bool BBox::onDragLeftMouse(const InputValue& inputValue)
{
	mBeforePoint = mCurrentPoint;
	mCurrentPoint = inputValue.get<Vec2>();

	if (rTarget.isNull() || mCurrentControlType == ControlTypeCount)
	{
		return false;
	}

	mIsDrag = mControlBox[mCurrentControlType]->onLeftDrag();
	return mIsDrag;
}
bool BBox::onEndLeftMouse(const InputValue& inputValue)
{
	mIsDrag = false;
	// todo: undo/redo event & keyframe
	if (mCurrentControlType == ControlTypeCount)
	{
		return false;
	}

	UpdateEntityEnd(rTarget.getComponent<IDComponent>().id);

	mCurrentControlType = ControlTypeCount;
	return true;
}

bool BBox::onMoveMouse(const InputValue& inputValue)
{
	if (mIsDrag)
		return true;

	if (rTarget.isNull() || !rTarget.hasComponent<ShapeComponent>())
		return false;

	auto& shape = rTarget.getComponent<ShapeComponent>();
	return IsInner(shape.shape, inputValue.get<Vec2>());
}

}	 // namespace core