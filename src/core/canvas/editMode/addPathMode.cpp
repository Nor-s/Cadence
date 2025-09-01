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
	if (!mContext.line.isNull())
	{
		rCanvas->mScene->destroyEntity(mContext.line);
	}
}

bool AddPathMode::onStarClickLefttMouse(const InputValue& inputValue)
{
	mContext.beforePos = mContext.currentPos = mContext.startPos = inputValue.get<Vec2>();
	auto& path = mContext.path;
	auto& control = mContext.controlList;

	mContext.clickedIndex = -1;
	mContext.pickedControl = nullptr;

	if (mContext.leftControl)
	{
		if (mContext.leftControl->onLeftDown(mContext.currentPos))
		{
			mContext.pickedControl = mContext.leftControl.get();
			return true;
		}

		if (mContext.rightControl->onLeftDown(mContext.currentPos))
		{
			mContext.pickedControl = mContext.rightControl.get();
			return true;
		}
	}

	for (int i = 0; i < mContext.controlList.size(); i++)
	{
		auto& c = mContext.controlList[i];
		if (c->onLeftDown(mContext.currentPos))
		{
			mContext.pickedControl = c.get();
			mContext.clickedIndex = i;
			mContext.controlIndex = -1;
			updateCurrentPointControl();
			mContext.activePreview = false;
			return true;
		}
	}

	if (!mContext.line.isNull())
	{
		rCanvas->mScene->destroyEntity(mContext.line);
	}

	// if no pick
	{
		int idx = path.size();
		ControlOverlay::ShapeType shapeType = ControlOverlay::ShapeType::FillStrokeEllipse;
		path.push_back(PathPoint{.base = mContext.currentPos, .type = mContext.currentEditType});
		if (path.size() == 1)
		{
			shapeType = ControlOverlay::ShapeType::ActiveFillStrokeEllipse;
			path.back().type = PathPoint::Command::MoveTo;
		}
		updateCurrentPointControl();

		control.push_back(std::make_unique<ControlOverlay>(rCanvas->mControlScene.get(), mContext.currentPos,
														   CommonSetting::Width_DefaultPathPointControlBox,
														   ControlOverlay::Type::Move, shapeType));
		control.back()->setOnLeftDrag(MakeLambda(
			[ctx = &mContext, idx, c = control.back().get()]()
			{
				auto delta = (ctx->currentPos - ctx->beforePos);
				c->moveByDelta(delta);
				ctx->path[idx].base = ctx->path[idx].base + delta;
				return true;
			}));
		mContext.activePreview = true;
		mContext.controlIndex = -1;
	}
	mContext.line = rCanvas->mScene->createPathLayer(mContext.path);

	return false;
}

bool AddPathMode::onDragLeftMouse(const InputValue& inputValue)
{
	bool ret = false;
	mContext.beforePos = mContext.currentPos;
	mContext.currentPos = inputValue.get<Vec2>();
	auto& path = mContext.path;

	if (!mContext.line.isNull())
	{
		rCanvas->mScene->destroyEntity(mContext.line);
	}

	if (mContext.pickedControl != nullptr)
	{
		ret = mContext.pickedControl->onLeftDrag();
	}
	else if (path.size() > 1)
	{
		auto len = length2(mContext.currentPos - mContext.startPos);
		if (len > CommonSetting::Threshold_AddPathModeChangeCurve)
		{
			mContext.currentEditType = PathPoint::Command::CubicTo;
			auto delta = mContext.currentPos - mContext.startPos;
			path.back().type = PathPoint::Command::CubicTo;
			path.back().deltaLeftControl = delta * -1.0f;
			path.back().deltaRightControl = delta;
		}
		ret = true;
	}

	mContext.line = rCanvas->mScene->createPathLayer(mContext.path);
	updateCurrentPointControl();
	return ret;
}

bool AddPathMode::onMoveMouse(const InputValue& inputValue)
{
	if (mContext.activePreview == false || mContext.clickedIndex >= 0 || mContext.path.empty())
	{
		if (mContext.preview)
		{
			mContext.preview.reset();
		}
		return false;
	}

	mContext.preview =
		std::make_unique<ControlOverlay>(rCanvas->mOverlayScene.get(), mContext.path.back(),
										 PathPoint{.base = inputValue.get<Vec2>(), .type = mContext.currentEditType});

	return false;
}

bool AddPathMode::onEndLeftMouse(const InputValue& inputValue)
{
	mContext.clickedIndex = -1;
	mContext.pickedControl = nullptr;

	auto& path = mContext.path;
	auto len = length2(mContext.currentPos - mContext.startPos);

	if (len < CommonSetting::Threshold_AddPathModeChangeCurve && !path.empty())
	{
		mContext.currentEditType = PathPoint::Command::LineTo;
	}
	return false;
}

bool AddPathMode::onInputAttach(const InputValue& inputValue)
{
	return false;
}

bool AddPathMode::onInputDetach(const InputValue& inputValue)
{
	mContext.clickedIndex = -1;
	mContext.pickedControl = nullptr;
	return false;
}

void AddPathMode::genCurrentPointControl()
{
	PathPoint p1;
	int currentIndex = -1;

	if (mContext.clickedIndex != -1)
	{
		currentIndex = mContext.clickedIndex;
		p1 = mContext.path[mContext.clickedIndex];
	}
	else if (mContext.controlIndex != -1)
	{
		currentIndex = mContext.controlIndex;
		p1 = mContext.path[mContext.controlIndex];
	}
	else if (mContext.path.size() > 0)
	{
		p1 = mContext.path.back();
		currentIndex = mContext.path.size() - 1;
	}
	if (p1.type != PathPoint::Command::CubicTo || currentIndex < 1)
	{
		mContext.controlLine = nullptr;
		mContext.rightControl = nullptr;
		mContext.leftControl = nullptr;
		mContext.controlIndex = -1;
		return;
	}
	auto start = p1.base + p1.deltaLeftControl;
	auto end = p1.base + p1.deltaRightControl;
	auto startPathPoint = PathPoint{
		.base = start,
		.type = PathPoint::Command::MoveTo,
	};
	auto endPathPoint = PathPoint{
		.base = end,
		.type = PathPoint::Command::LineTo,
	};
	mContext.controlLine = std::make_unique<ControlOverlay>(rCanvas->mOverlayScene.get(), startPathPoint, endPathPoint);

	bool isLeftClicked = mContext.leftControl && mContext.pickedControl == mContext.leftControl.get();
	bool isRightClicked = mContext.rightControl && mContext.pickedControl == mContext.rightControl.get();

	mContext.leftControl = std::make_unique<ControlOverlay>(
		rCanvas->mControlScene.get(), start, CommonSetting::Width_DefaultPathPointControlBox,
		ControlOverlay::Type::Move, ControlOverlay::ShapeType::FillStrokeRect);

	mContext.leftControl->setOnLeftDrag(MakeLambda(
		[ctx = &mContext, idx = currentIndex, c = mContext.leftControl.get()]()
		{
			auto delta = (ctx->currentPos - ctx->beforePos);
			c->moveByDelta(delta);
			ctx->path[idx].deltaLeftControl = ctx->path[idx].deltaLeftControl + delta;
			ctx->path[idx].deltaRightControl = ctx->path[idx].deltaLeftControl * -1.0f;
			return true;
		}));

	mContext.rightControl = std::make_unique<ControlOverlay>(
		rCanvas->mControlScene.get(), end, CommonSetting::Width_DefaultPathPointControlBox, ControlOverlay::Type::Move,
		ControlOverlay::ShapeType::FillStrokeRect);

	mContext.rightControl->setOnLeftDrag(MakeLambda(
		[ctx = &mContext, idx = currentIndex, c = mContext.rightControl.get()]()
		{
			auto delta = (ctx->currentPos - ctx->beforePos);
			c->moveByDelta(delta);
			ctx->path[idx].deltaRightControl = ctx->path[idx].deltaRightControl + delta;
			ctx->path[idx].deltaLeftControl = ctx->path[idx].deltaRightControl * -1.0f;
			return true;
		}));
	if (isLeftClicked)
		mContext.pickedControl = mContext.leftControl.get();
	if (isRightClicked)
		mContext.pickedControl = mContext.rightControl.get();
	mContext.controlIndex = currentIndex;
}

void AddPathMode::updateCurrentPointControl()
{
	genCurrentPointControl();

	if (mContext.leftControl)
	{
		// auto& p1 = mContext.path[mContext.clickedIndex];
		// mContext.leftControl->moveTo(p1.base + p1.deltaLeftControl);
		// mContext.rightControl->moveTo(p1.base + p1.deltaRightControl);
	}
}

void AddPathMode::onUpdate()
{
}

inline AddPathMode::Context::Context()
{
}

inline AddPathMode::Context::~Context()
{
}

}	 // namespace core