#ifndef _CORE_CANVAS_ANIMATION_CREATOR_CANVAS_H_
#define _CORE_CANVAS_ANIMATION_CREATOR_CANVAS_H_

#include "canvas.h"

namespace core
{

class Scene;
class Animator;
class AnimationCreatorInputController;

class AnimationCreatorCanvas : public CanvasWrapper
{
public:
	AnimationCreatorCanvas(void* context, Size size, bool bIsSw);
	~AnimationCreatorCanvas() = default;

	CanvasType type() override
	{
		return CanvasType::LottieCreator;
	}

	InputController* getInputController() override;

	void onUpdate() override;

	std::unique_ptr<core::Animator> mAnimator;
	std::unique_ptr<core::Scene> mScene;
	std::unique_ptr<core::Scene> mOverlayScene;
	std::unique_ptr<core::Scene> mControlScene;
	std::unique_ptr<AnimationCreatorInputController> mInputController;
};

}	 // namespace core

#endif