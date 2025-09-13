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
	mCanvasScene = std::make_unique<core::Scene>();
	mMainScene = std::make_unique<core::Scene>(mCanvasScene.get());
	mControlScene = std::make_unique<core::Scene>();
	mAnimator = std::make_unique<core::Animator>(this);

	mCanvasScene->pushCanvas(this);
	mCanvas->push(mCanvasScene->mTvgScene);

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
	mIsDirty |= mMainScene->onUpdate();
	mInputController->onUpdate();
	mIsDirty |= mControlScene->onUpdate();
}
void AnimationCreatorCanvas::onDestroy()
{
	mMainScene->destroy();
	mControlScene->destroy();
}
}	 // namespace core