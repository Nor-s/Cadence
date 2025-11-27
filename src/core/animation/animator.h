#ifndef _CORE_ANIMATION_MANAGER_H_
#define _CORE_ANIMATION_MANAGER_H_

#include <numeric>
#include <stdint.h>

namespace core
{

class AnimationCreatorCanvas;

class Animator
{
public:
	Animator(AnimationCreatorCanvas* canvas);
	~Animator();

	void play();
	void stop();
	void update();

	uint32_t mMinFrameNo{0u};
	uint32_t mMaxFrameNo{200u};
	float mCurrentFrameNo{0.0f};
	uint32_t mFps{24u};
	float mSpeed{1.0f};

	bool mIsStop{true};
	bool mDirty{false};

private:
	AnimationCreatorCanvas* rCanvas{nullptr};
};

}	 // namespace core

#endif