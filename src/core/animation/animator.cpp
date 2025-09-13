#include "animation/animator.h"

#include "canvas/animationCreatorCanvas.h"

#include "system/io.h"

namespace core
{

Animator::Animator(AnimationCreatorCanvas* canvas) : rCanvas(canvas)
{
}

Animator::~Animator()
{
}

void Animator::play()
{
	mIsStop = false;
}
void Animator::stop()
{
	mIsStop = true;
	mCurrentFrameNo = static_cast<uint32_t>(mCurrentFrameNo);
}

void Animator::update()
{
	if (mIsStop)
		return;

	mCurrentFrameNo += io::deltaTime * mFps * mSpeed;

	if (mSpeed < 0 && mCurrentFrameNo < 0.0f)
	{
		mCurrentFrameNo = mMaxFrameNo;
	}
	else if (mSpeed > 0 && mCurrentFrameNo > mMaxFrameNo)
	{
		mCurrentFrameNo = mMinFrameNo;
	}
}

}	 // namespace core