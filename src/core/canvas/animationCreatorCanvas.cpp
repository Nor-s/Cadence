#include "animationCreatorCanvas.h"

#include "core/input/inputController.h"
#include "core/input/inputAction.h"
#include "animationCreatorInputController.h"

#include "scene/scene.h"

#include "animation/animator.h"

#include "scene/component/components.h"
#include "selection/selectionManager.h"

#include <memory>

namespace core
{

AnimationCreatorCanvas::AnimationCreatorCanvas(void* context, Size size, bool bIsSw)
	: CanvasWrapper(context, size, bIsSw)
{
	mCanvasScene = std::make_unique<core::Scene>();
	auto board = mCanvasScene->createRectFillLayer({-256, -256}, {512, 512});
	auto& fill = board.getComponent<SolidFillComponent>();
	fill.color = {255, 255, 255};
	board.update();

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
	mIsDirty |= mCanvasScene->onUpdate();
	mInputController->onUpdate();
	SelectionManager::Update(this);
	mIsDirty |= mControlScene->onUpdate();
}
void AnimationCreatorCanvas::onDestroy()
{
	mCanvasScene->destroy();
	mControlScene->destroy();
}
void AnimationCreatorCanvas::moveCamera(Vec2 xy)
{
	mCanvasScene->mSceneEntity.move(xy);
}

}	 // namespace core