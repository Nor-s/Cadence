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
		return CanvasType::AnimationCreator;
	}

	InputController* getInputController() override;

	void onUpdate() override;
	void onDestroy() override;
	void moveCamera(Vec2 xy) override;

	std::unique_ptr<core::Scene> mCanvasScene;	  // todo: mCanvasScene Move to CanvasWrapper
	std::unique_ptr<core::Animator> mAnimator;
	std::unique_ptr<core::Scene> mMainScene;
	std::unique_ptr<core::Scene> mControlScene;
	std::unique_ptr<AnimationCreatorInputController> mInputController;
};

}	 // namespace core

#endif