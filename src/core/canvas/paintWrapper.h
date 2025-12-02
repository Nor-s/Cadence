#ifndef _CORE_CANVAS_PAINT_WRAPPER_H_
#define _CORE_CANVAS_PAINT_WRAPPER_H_

#include "common/common.h"
#include <thorvg.h>
#include <thorvg_lottie.h>

namespace core
{
class PaintWrapper
{
public:
	// todo: add dtor
	virtual ~PaintWrapper()
	{
		if (mHandle)
			mHandle->unref();
	}

	void translate(const Vec2& xy)
	{
		mHandle->translate(xy.x, xy.y);
	}

	virtual void scale(const Size& size)
	{
		return;
	}

	static void PictureScale(tvg::Picture* picture, const Size& size)
	{
		float scale = 0.0f, w2 = 0.0f, h2 = 0.0f;
		picture->size(&w2, &h2);

		if (w2 > h2)
			scale = size.x / w2;
		else
			scale = size.y / h2;

		picture->scale(scale);
		return;
	}

	tvg::Paint* mHandle = nullptr;

protected:
	PaintWrapper() = default;
};

class PictureWrapper : public PaintWrapper
{
public:
	static std::unique_ptr<PictureWrapper> Gen(tvg::Picture* picture)
	{
		auto pictureWrapper = std::make_unique<PictureWrapper>();

		pictureWrapper->mHandle = picture;
		pictureWrapper->mHandle->ref();

		return pictureWrapper;
	}
	static std::unique_ptr<PictureWrapper> Gen(std::string_view path)
	{
		auto* picture = tvg::Picture::gen();
		auto pictureWrapper = Gen(picture);

		picture->load(path.data());

		return pictureWrapper;
	}

public:
	~PictureWrapper() = default;

	void scale(const Size& size) override
	{
		auto* picture = static_cast<tvg::Picture*>(mHandle);
		PaintWrapper::PictureScale(picture, size);
	}
};

class AnimationWrapper
{
public:
	static std::pair<std::unique_ptr<AnimationWrapper>, std::unique_ptr<PictureWrapper>> Gen(std::string_view path)
	{
		auto animationWrapper = std::make_unique<AnimationWrapper>();
		auto* anim = animationWrapper->mHandle = tvg::LottieAnimation::gen();
		auto* picture = anim->picture();

		picture->load(path.data());

		return std::make_pair(std::move(animationWrapper), PictureWrapper::Gen(picture));
	}
	inline static float Progress(uint32_t elapsed, float durationInSec, bool rewind = false)
	{
		auto duration = uint32_t(durationInSec * 1000.0f);	  // sec -> millisec.
		if (elapsed == 0 || duration == 0)
			return 0.0f;
		auto forward = ((elapsed / duration) % 2 == 0) ? true : false;
		if (elapsed % duration == 0)
			return 1.0f;
		auto progress = (float(elapsed % duration) / (float) duration);
		if (rewind)
			return forward ? progress : (1 - progress);
		return progress;
	}

public:
	~AnimationWrapper() = default;

	void frame(uint32_t elapsed)
	{
		auto progress = Progress(elapsed, mHandle->duration());
		mHandle->frame(mHandle->totalFrame() * progress);
	}
	tvg::LottieAnimation* handle()
	{
		return mHandle;
	}

private:
	tvg::LottieAnimation* mHandle;
};

}	 // namespace core

#endif