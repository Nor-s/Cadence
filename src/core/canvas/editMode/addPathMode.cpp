#include "addPathMode.h"
#include "scene/ui/controlBox.h"
#include "canvas/animationCreatorCanvas.h"
#include "scene/component/components.h"

namespace core
{

AddPathMode::AddPathMode(AnimationCreatorCanvas* canvas)
{
	rCanvas = canvas;
	onInputAttach({1.0f, 1.0f});
}

AddPathMode::~AddPathMode()
{
}

bool AddPathMode::onStarClickLefttMouse(const InputValue& inputValue)
{
	mContext.beforePos = mContext.currentPos = mContext.startPos = inputValue.get<Vec2>();
	auto& path = mContext.pathList;
	auto& control = mContext.controlList;
	auto* scene = rCanvas->mOverlayScene.get();

	for (auto& p : path)
	{
		// scene->createPolygonFillLayer
	}

	// todo:: pick control?
	mContext.clickedControl = nullptr;
	for (auto& c : control)
	{
		if (c->onLeftDown(mContext.currentPos))
		{
			mContext.clickedControl = c.get();
			return true;
		}
	}

	// if no pick
	{
		path.push_back(PathPoint{.base = mContext.currentPos});
		control.push_back(std::make_unique<ControlBox>(
			rCanvas->mOverlayScene.get(), mContext.currentPos, CommonSetting::Width_DefaultPathPointControlBox,
			ControlBox::Type::Move, ControlBox::ShapeType::FillStrokeEllipse));
		control.back()->setOnLeftDrag(MakeLambda(
			[ctx = &mContext, p = &path.back(), c = control.back().get()]()
			{
				auto delta = (ctx->currentPos - ctx->beforePos);
				c->moveByDelta(delta);
				p->base = p->base + delta;
				return true;
			}));
	}

	return false;
}

bool AddPathMode::onDragLeftMouse(const InputValue& inputValue)
{
	mContext.beforePos = mContext.currentPos;
	mContext.currentPos = inputValue.get<Vec2>();

	if (mContext.clickedControl)
	{
		return mContext.clickedControl->onLeftDrag();
	}
	auto len = length2(mContext.currentPos - mContext.startPos);
	auto& path = mContext.pathList;
	if (len > CommonSetting::Threshold_AddPathModeChangeCurve)
	{
		path.back().type = PathPoint::Type::Curve;
		mContext.currentEditType = PathPoint::Type::Curve;
	}
	else
	{
		path.back().type = PathPoint::Type::Line;
		mContext.currentEditType = PathPoint::Type::Line;
	}

	return false;
}

bool AddPathMode::onMoveMouse(const InputValue& inputValue)
{
	return false;
}

bool AddPathMode::onEndLeftMouse(const InputValue& inputValue)
{
	mContext.clickedControl = nullptr;
	return false;
}

bool AddPathMode::onInputAttach(const InputValue& inputValue)
{
	return false;
}

bool AddPathMode::onInputDetach(const InputValue& inputValue)
{
	mContext.clickedControl = nullptr;
	return false;
}

void AddPathMode::onUpdate()
{
}

}	 // namespace core