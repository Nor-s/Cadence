#include "addPathMode.h"
#include "scene/ui/controlBox.h"
#include "scene/ui/editPath.h"
#include "canvas/animationCreatorCanvas.h"
#include "scene/component/components.h"

namespace core
{

AddPathMode::AddPathMode(AnimationCreatorCanvas* canvas)
{
	rCanvas = canvas;
	init();

	onInputAttach({1.0f, 1.0f});
}

AddPathMode::~AddPathMode()
{
	if (!mTargetPath.isNull())
	{
		rCanvas->mScene->destroyEntity(mTargetPath);
	}
}

bool AddPathMode::onStarClickLefttMouse(const InputValue& inputValue)
{
	auto start = inputValue.get<Vec2>();

	if (!mEditPath)
	{
		PathPoints pathPoints;
		pathPoints.push_back(PathPoint{.localPosition = start, .type = PathPoint::Command::MoveTo});
		mTargetPath = rCanvas->mScene->createPathLayer(pathPoints);
		mEditPath = std::make_unique<EditPath>(rCanvas->mOverlayScene.get(), mTargetPath, true);
	}
	return mEditPath->onStartClickLeftMouse(inputValue);
}

bool AddPathMode::onDragLeftMouse(const InputValue& inputValue)
{
	if (!mEditPath)
		return false;

	return mEditPath->onDragLeftMouse(inputValue);
}

bool AddPathMode::onMoveMouse(const InputValue& inputValue)
{
	if (!mEditPath)
		return false;

	return mEditPath->onMoveMouse(inputValue);
}

bool AddPathMode::onEndLeftMouse(const InputValue& inputValue)
{
	if (!mEditPath)
		return false;

	if (mEditPath->onEndLeftMouse(inputValue))
	{
		mTargetPath = Entity();
		return true;
	}
	return false;
}

bool AddPathMode::onInputAttach(const InputValue& inputValue)
{
	return false;
}

bool AddPathMode::onInputDetach(const InputValue& inputValue)
{
	return false;
}

void AddPathMode::init()
{
}

void AddPathMode::onUpdate()
{
	if (mEditPath)
	{
		mEditPath->onUpdate();
	}
}

}	 // namespace core