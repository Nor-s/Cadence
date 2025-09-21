#include "editPath.h"

#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/ui/controlBox.h"
#include "scene/component/components.h"

#include "canvas/animationCreatorCanvas.h"
#include "canvas/shapeUtil.h"

#include "controlBox.h"

#include "common/common.h"

#include "interface/editInterface.h"
#include "interface/editHelper.h"
#include "input/input.h"
#include "input/inputController.h"

#include "system/io.h"

namespace core
{

EditPath::EditPath(InputController* inputController, core::Scene* scene, Entity target, int pathIndex)
	: rInputController(inputController)
	, rScene(scene)
	, rTarget(target)
	, mIsAddMode(pathIndex == -1)
	, mPathIndex(pathIndex)
{
	mInputActionBindings.push_back(inputController->bindAction(InputAction(InputType::MOUSE_LEFT_DOWN, 2, true),
															   InputTrigger::Started, this,
															   &EditPath::onStartClickLeftMouse));
	mInputActionBindings.push_back(inputController->bindAction(
		InputAction(InputType::MOUSE_LEFT_DOWN, 2, true), InputTrigger::Triggered, this, &EditPath::onDragLeftMouse));
	mInputActionBindings.push_back(inputController->bindAction(InputAction(InputType::MOUSE_LEFT_DOWN, 2, true),
															   InputTrigger::Ended, this, &EditPath::onEndLeftMouse));
	mInputActionBindings.push_back(inputController->bindAction(InputAction(InputType::MOUSE_MOVE, 2, true),
															   InputTrigger::Triggered, this, &EditPath::onMoveMouse));
	if (mPathIndex == -1)
		mPathIndex = 0;

	init();
}
EditPath::~EditPath()
{
	for (auto& binding : mInputActionBindings)
	{
		rInputController->unbinding(binding);
	}
}
void EditPath::init()
{
	initPreview();
	initControlPoint();
	initPath();
}
void EditPath::onUpdate()
{
	if (rTarget.isNull())
	{
		return;
	}
}

bool EditPath::onStartClickLeftMouse(const InputValue& inputValue)
{
	mIsAddPoint = false;
	rCurrentUi = nullptr;
	mIsDrag = true;
	mStartPoint = mBeforePoint = inputValue.get<Vec2>();
	mElasped = 0;

	// move control points
	if (onStartClickControlForCurvePoint())
	{
		return true;
	}

	if (onStartClickForPathPoint())
	{
		return true;
	}
	addPoint(mStartPoint);

	return true;
}

bool EditPath::onDragLeftMouse(const InputValue& inputValue)
{
	mElasped += io::deltaTime;
	mBeforePoint = mCurrentPoint;
	mCurrentPoint = inputValue.get<Vec2>();

	if (rCurrentUi)
	{
		if (mIsAddPoint)
		{
			auto len = length2(mCurrentPoint - mStartPoint);
			if (len > CommonSetting::Threshold_AddPathModeChangeCurve ||
				mCurrentEditType == PathPoint::Command::CubicTo)
			{
				mCurrentEditType = PathPoint::Command::CubicTo;
				auto& transform = rTarget.getComponent<WorldTransformComponent>();
				auto localStart = mStartPoint * transform.inverseWorldTransform;
				auto localCurrent = mCurrentPoint * transform.inverseWorldTransform;
				auto localDelta = localCurrent - localStart;

				auto point = ToEdit_PathPoint(rPathPoints->at(mCurrentPointIndex));
				if (point.type != EDIT_PathPointType_MoveTo)
					point.type = EDIT_PathPointType_CubicTo;

				point.leftControlRelPosition[0] = -localDelta.x;
				point.leftControlRelPosition[1] = -localDelta.y;
				point.rightControlRelPosition[0] = localDelta.x;
				point.rightControlRelPosition[1] = localDelta.y;
				Internal_Path_UpdatePathPoint(rTarget.getId(), mPathIndex, &point, mIsAddMode, mCurrentPointIndex);

				updateControlPoint();
			}

			return true;
		}
		else if (rCurrentUi->onDragLeftMouse())
		{
			return true;
		}
	}

	return true;
}
bool EditPath::onEndLeftMouse(const InputValue& inputValue)
{
	mIsDrag = false;

	for (auto& ui : mPathPointUIs)
	{
		ui->onEndLeftMouse();
	}
	mLeftControlUI->onEndLeftMouse();
	mRightControlUI->onEndLeftMouse();

	if (mIsAddMode)
	{
		if (rPathPoints->size() > 1 && mElasped < CommonSetting::Threshold_AddPathLayer &&
			rCurrentUi == mPathPointUIs.begin()->get())
		{
			return false;
		}
		if (mIsAddPoint)
		{
			auto len = length2(mCurrentPoint - mStartPoint);
			auto& p = rPathPoints->at(mCurrentPointIndex);
			if (len < CommonSetting::Threshold_AddPathModeChangeCurve)
			{
				mCurrentEditType = PathPoint::Command::LineTo;
			}
		}
	}
	return true;
}

bool EditPath::onMoveMouse(const InputValue& inputValue)
{
	auto point = inputValue.get<Vec2>();
	if (mIsAddMode)
	{
		updatePreview(point);
	}
	for (auto& ui : mPathPointUIs)
	{
		ui->onMoveMouse(point);
	}
	mLeftControlUI->onMoveMouse(point);
	mRightControlUI->onMoveMouse(point);

	return true;
}

void EditPath::addPoint(const Vec2& worldPoisition)
{
	if (mIsAddMode)
	{
		// add new point
		mCurrentPointIndex = rPathPoints->size();
		addPathPoint(worldPoisition);
		mIsAddPoint = true;
		rCurrentUi = mPathPointUIs.back().get();
	}
}

}	 // namespace core

// private
namespace core
{
void EditPath::initControlPoint()
{
	// line
	{
		mControlLineUI = std::make_unique<UIShape>(rScene, PathPoint{.localPosition = {0.0f, 0.0f}},
												   PathPoint{.localPosition = {1.0f, 1.0f}});
		mControlLineUI->setVisible(false);

		mControlLineUI->setOnUpdate(MakeLambda(
			[this]()
			{
				PathPoints pathPoints;
				pathPoints.push_back(
					PathPoint{.localPosition = mLeftControlUI->getWorldPosition(), .type = PathPoint::Command::MoveTo});
				pathPoints.push_back(PathPoint{.localPosition = mRightControlUI->getWorldPosition(),
											   .type = PathPoint::Command::LineTo});
				mControlLineUI->updatePath(pathPoints);
				return true;
			}));
	}
	UIShape::Attribute attribute{};
	attribute.cursorType = UIShape::CursorType::Move;
	attribute.shapeType = UIShape::ShapeType::Rect;
	attribute.shapeAtt = UIShape::ShapeAttribute::FillStroke;
	attribute.hovered.fillColor = CommonSetting::Color_DefaultActiveFillStrokeEllipse;
	attribute.hovered.fillAlpha = 125.0f;
	attribute.pressed.fillColor = CommonSetting::Color_DefaultActiveFillStrokeEllipse;

	// left
	{
		mLeftControlUI = std::make_unique<UIShape>(rScene, Vec2{0.0f, 0.0f},
												   CommonSetting::Width_DefaultPathPointControlBox, attribute);
		mLeftControlUI->setVisible(false);
		mLeftControlUI->setOnLeftDrag(MakeLambda(
			[this]()
			{
				auto& transform = rTarget.getComponent<WorldTransformComponent>();
				auto localCurrent = mCurrentPoint * transform.inverseWorldTransform;
				auto localBefore = mBeforePoint * transform.inverseWorldTransform;
				auto localDelta = (localCurrent - localBefore);
				auto delta = (mCurrentPoint - mBeforePoint);
				auto point = ToEdit_PathPoint(rPathPoints->at(mCurrentPointIndex));

				mLeftControlUI->moveByDelta(delta);
				mRightControlUI->moveByDelta(delta * -1.0f);

				point.leftControlRelPosition[0] += localDelta.x;
				point.leftControlRelPosition[1] += localDelta.y;
				point.rightControlRelPosition[0] -= localDelta.x;
				point.rightControlRelPosition[1] -= localDelta.y;

				Internal_Path_UpdatePathPoint(rTarget.getId(), mPathIndex, &point, mIsAddMode, mCurrentPointIndex);

				mControlLineUI->onUpdate();
				return true;
			}));
	}
	// right
	{
		attribute.shapeType = UIShape::ShapeType::Rect;

		mRightControlUI = std::make_unique<UIShape>(rScene, Vec2{0.0f, 0.0f},
													CommonSetting::Width_DefaultPathPointControlBox, attribute);
		mRightControlUI->setVisible(false);
		mRightControlUI->setOnLeftDrag(MakeLambda(
			[this]()
			{
				auto& transform = rTarget.getComponent<WorldTransformComponent>();
				auto localCurrent = mCurrentPoint * transform.inverseWorldTransform;
				auto localBefore = mBeforePoint * transform.inverseWorldTransform;
				auto localDelta = (localCurrent - localBefore);
				auto delta = (mCurrentPoint - mBeforePoint);
				auto point = ToEdit_PathPoint(rPathPoints->at(mCurrentPointIndex));

				mRightControlUI->moveByDelta(delta);
				mLeftControlUI->moveByDelta(delta * -1.0f);

				point.leftControlRelPosition[0] -= localDelta.x;
				point.leftControlRelPosition[1] -= localDelta.y;
				point.rightControlRelPosition[0] += localDelta.x;
				point.rightControlRelPosition[1] += localDelta.y;

				Internal_Path_UpdatePathPoint(rTarget.getId(), mPathIndex, &point, mIsAddMode, mCurrentPointIndex);

				mControlLineUI->onUpdate();
				return true;
			}));
	}
}

void EditPath::addPathPointControl(const Vec2& worldPosition)
{
	UIShape::Attribute attribute{};
	attribute.shapeType = UIShape::ShapeType::Ellipse;
	attribute.shapeAtt = UIShape::ShapeAttribute::FillStroke;
	attribute.cursorType = UIShape::CursorType::Move;
	attribute.hovered.fillColor = CommonSetting::Color_DefaultActiveFillStrokeEllipse;
	attribute.hovered.fillAlpha = 125.0f;
	attribute.pressed.fillColor = CommonSetting::Color_DefaultActiveFillStrokeEllipse;

	if (mPathPointUIs.empty())
	{
		attribute.normal.fillColor = CommonSetting::Color_DefaultActiveFillStrokeEllipse;
	}
	mPathPointUIs.push_back(
		std::make_unique<UIShape>(rScene, worldPosition, CommonSetting::Width_DefaultPathPointControlBox, attribute));

	mPathPointUIs.back()->setOnLeftDrag(MakeLambda(
		[this, idx = mPathPointUIs.size() - 1]()
		{
			auto& transform = rTarget.getComponent<WorldTransformComponent>();
			auto localCurrent = mCurrentPoint * transform.inverseWorldTransform;
			auto localBefore = mBeforePoint * transform.inverseWorldTransform;

			auto localDelta = localCurrent - localBefore;
			auto delta = (mCurrentPoint - mBeforePoint);
			mPathPointUIs[idx]->moveByDelta(delta);

			auto point = ToEdit_PathPoint(rPathPoints->at(idx));

			point.localPosition[0] += localDelta.x;
			point.localPosition[1] += localDelta.y;

			Internal_Path_UpdatePathPoint(rTarget.getId(), mPathIndex, &point, mIsAddMode, idx);

			updateControlPoint();

			return true;
		}));
}
void EditPath::addPathPoint(const Vec2& worldPoisition)
{
	auto& transform = rTarget.getComponent<WorldTransformComponent>();
	auto localPosition = worldPoisition * transform.inverseWorldTransform;
	Edit_PathPoint pathPoint{
		.localPosition = {localPosition.x, localPosition.y},
		.type = static_cast<Edit_PathPointType>(rPathPoints->empty() ? PathPoint::Command::MoveTo : mCurrentEditType)};

	Internal_Path_AddPathPoint(rTarget.getId(), mPathIndex, &pathPoint, mIsAddMode);

	addPathPointControl(worldPoisition);
}

bool EditPath::onStartClickControlForCurvePoint()
{
	if (mLeftControlUI->onStartLeftDown(mStartPoint))
	{
		rCurrentUi = mLeftControlUI.get();
		return mLeftControlUI->onDragLeftMouse();
	}
	else if (mRightControlUI->onStartLeftDown(mStartPoint))
	{
		rCurrentUi = mRightControlUI.get();
		return mRightControlUI->onDragLeftMouse();
	}
	return false;
}

bool EditPath::onStartClickForPathPoint()
{
	for (int i = 0; i < mPathPointUIs.size(); i++)
	{
		auto& control = mPathPointUIs[i];
		if (control->onStartLeftDown(mStartPoint))
		{
			mCurrentPointIndex = i;
			rCurrentUi = control.get();
			return control->onDragLeftMouse();
		}
	}

	return false;
}

void EditPath::updateControlPoint()
{
	auto& point = rPathPoints->at(mCurrentPointIndex);
	if (point.type == PathPoint::Command::CubicTo ||
		(point.type == PathPoint::Command::MoveTo && mCurrentEditType == PathPoint::Command::CubicTo))
	{
		auto& transform = rTarget.getComponent<WorldTransformComponent>();
		auto worldPos = point.localPosition * transform.worldTransform;
		auto leftWorldPos = (point.localPosition + point.deltaLeftControlPosition) * transform.worldTransform;
		auto rightWorldPos = (point.localPosition + point.deltaRightControlPosition) * transform.worldTransform;

		mRightControlUI->setVisible(true);
		mLeftControlUI->setVisible(true);
		mControlLineUI->setVisible(true);
		mRightControlUI->moveTo(rightWorldPos);
		mLeftControlUI->moveTo(leftWorldPos);
		mControlLineUI->onUpdate();
	}
	else
	{
		mRightControlUI->setVisible(false);
		mLeftControlUI->setVisible(false);
		mControlLineUI->setVisible(false);
	}
}

void EditPath::updatePreview(const Vec2& endPoint)
{
	if (!mIsAddMode)
		return;

	if (mCurrentPointIndex == (rPathPoints->size() - 1) && rCurrentUi == mPathPointUIs.back().get())
	{
		PathPoints pathPoints;
		auto world = rTarget.getComponent<WorldTransformComponent>().worldTransform;
		auto& point = rPathPoints->back();
		auto beforeWorld = point.localPosition * world;
		auto beforeLeftWorld = (point.localPosition + point.deltaLeftControlPosition) * world - beforeWorld;
		auto beforeRightWorld = (point.localPosition + point.deltaRightControlPosition) * world - beforeWorld;
		auto type = rPathPoints->back().type;

		pathPoints.push_back(PathPoint{.localPosition = beforeWorld,
									   .deltaLeftControlPosition = beforeLeftWorld,
									   .deltaRightControlPosition = beforeRightWorld,
									   .type = type});
		pathPoints.push_back(PathPoint{.localPosition = endPoint, .type = mCurrentEditType});
		mPathPreviewUI->updatePath(pathPoints);
		mPathPreviewUI->setVisible(true);
	}
	else
	{
		mPathPreviewUI->setVisible(false);
	}
}

void EditPath::initPreview()
{
	UIShape::Attribute att{};
	att.normal.strokeWidth = CommonSetting::Width_DefaultPathLine;

	mPathPreviewUI = std::make_unique<UIShape>(rScene, PathPoint{.localPosition = {0.0f, 0.0f}},
											   PathPoint{.localPosition = {0.0f, 0.0f}}, att);
	mPathPreviewUI->setVisible(false);
}

void EditPath::initPath()
{
	mPathPointUIs.clear();
	auto* pathComponent = rTarget.getPath<RawPath>(mPathIndex);
	if (pathComponent == nullptr)
		return;

	auto& transformComponent = rTarget.getComponent<WorldTransformComponent>();
	auto world = transformComponent.worldPosition;
	rPathPoints = &pathComponent->path;

	for (auto& point : pathComponent->path)
	{
		addPathPointControl(point.localPosition + world);
	}
}

}	 // namespace core