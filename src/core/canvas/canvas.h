#ifndef _CORE_CANVAS_CANVAS_H_
#define _CORE_CANVAS_CANVAS_H_

#include "paintWrapper.h"

#include <thorvg.h>

#include "common/common.h"
#include <vector>
#include <memory>

class GlRenderTarget;
namespace core
{
class InputController;

enum class CanvasType
{
	Base,
	Example,
	AnimationCreator
};

// todo: delete move, copy
class CanvasWrapper
{
public:
public:
	CanvasWrapper(void* context, Size size, bool bIsSw);
	virtual ~CanvasWrapper();

	void clearColor(float color[3])
	{
		memcpy(mClearColor, color, 3 * sizeof(float));
	}

	virtual void onInit(){};
	virtual void onUpdate();
	virtual void onDestroy()
	{
	}
	virtual void onResize(){};
	virtual CanvasType type()
	{
		return CanvasType::Base;
	}

	void draw();
	void resize(Size size);
	void update();
	void setDirty(bool dirty);
	uint32_t getTexture();

	tvg::Canvas* getCanvas()
	{
		return mCanvas;
	}
	unsigned char* getBuffer();

	virtual void pushPaint(std::unique_ptr<PaintWrapper> paint)
	{
		paint->scale(mSize);
		mCanvas->push(paint->mHandle);
		mCanvas->update();

		mPaints.push_back(std::move(paint));
	}

	virtual void pushAnimation(std::unique_ptr<AnimationWrapper> anim)
	{
		mAnimations.push_back(std::move(anim));
	}

	virtual InputController* getInputController();

	bool isSw()
	{
		return mIsSw;
	}
	virtual void moveCamera(Vec2 xy);

	Size mSize{};

	uint32_t mGlobalElapsed = 0;

protected:
	// todo: smart pointer
	GlRenderTarget* mRenderTarget{};
	tvg::Canvas* mCanvas{nullptr};

	float mClearColor[3]{};
	void* rContext{nullptr};
	Size mBeforeSize;
	bool mIsSw = false;

	unsigned char* mBuffer = nullptr;
	uint32_t* mSwBuffer = nullptr;

	// imported image, svg, lottie..
	std::vector<std::unique_ptr<PaintWrapper>> mPaints;
	std::vector<std::unique_ptr<AnimationWrapper>> mAnimations;

	bool mIsDirty{false};
};

}	 // namespace core

#endif