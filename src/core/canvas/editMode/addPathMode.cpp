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
}

AddPathMode::~AddPathMode()
{
	if (!mTargetPath.isNull())
	{
		rCanvas->mMainScene->destroyEntity(mTargetPath);
	}
}

bool AddPathMode::onStarClickLefttMouse(const InputValue& inputValue)
{
	auto start = inputValue.get<Vec2>();

	if (!mEditPath)
	{
		PathPoints pathPoints;
		auto& world = rCanvas->mMainScene->mSceneEntity.getComponent<WorldTransformComponent>();
		auto pos = start * world.inverseWorldTransform;
		mTargetPath = rCanvas->mMainScene->createPathLayer(pathPoints);
		mEditPath =
			std::make_unique<EditPath>(rCanvas->getInputController(), rCanvas->mControlScene.get(), mTargetPath);
	}
	return mEditPath->onStartClickLeftMouse(inputValue);
}

bool AddPathMode::onDragLeftMouse(const InputValue& inputValue)
{
	return false;
}

bool AddPathMode::onMoveMouse(const InputValue& inputValue)
{
	return false;
}

bool AddPathMode::onEndLeftMouse(const InputValue& inputValue)
{
	if (!mEditPath)
		return false;

	{
		// todo: this logic to edit path?
		// mTargetPath is Delete when Change Edit Mode
		auto* rawPath = static_cast<RawPath*>(mTargetPath.getComponent<PathListComponent>().paths[0].get());
		rawPath->path.push_back(PathPoint{.type = PathPoint::Command::Close});
		if (!mTargetPath.hasComponent<SolidFillComponent>())
			mTargetPath.addComponent<SolidFillComponent>();
		Resolve(mTargetPath.getComponent<TransformComponent>(), *rawPath);

		mTargetPath.setDirty(Dirty::Type::Path | Dirty::Type::Transform);

		mTargetPath = Entity();
	}
	return true;
	// return false;
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
		mEditPath->onUpdate();
}

}	 // namespace core