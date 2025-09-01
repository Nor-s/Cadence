#include "animationCreatorCanvas.h"

#include "core/input/inputController.h"
#include "core/input/inputAction.h"
#include "animationCreatorInputController.h"

#include "scene/scene.h"

#include "animation/animator.h"

#include <memory>

namespace core
{

AnimationCreatorCanvas::AnimationCreatorCanvas(void* context, Size size, bool bIsSw)
	: CanvasWrapper(context, size, bIsSw)
{
	mScene = std::make_unique<core::Scene>(true);
	mOverlayScene = std::make_unique<core::Scene>(false);
	mControlScene = std::make_unique<core::Scene>(false);
	mAnimator = std::make_unique<core::Animator>(this);

	mScene->pushCanvas(this);
	mCanvas->push(mScene->mTvgScene);

	mOverlayScene->pushCanvas(this);
	mCanvas->push(mOverlayScene->mTvgScene);

	mControlScene->pushCanvas(this);
	mCanvas->push(mControlScene->mTvgScene);

	mInputController = std::make_unique<AnimationCreatorInputController>(this);
}

InputController* AnimationCreatorCanvas::getInputController()
{
	if (mInputController)
		return mInputController->mHandle.get();
	return nullptr;
}

void AnimationCreatorCanvas::onUpdate()
{
	CanvasWrapper::onUpdate();

	mAnimator->update();
	mScene->onUpdate();
	mOverlayScene->onUpdate();
	mInputController->onUpdate();
	mControlScene->onUpdate();
}
}	 // namespace core