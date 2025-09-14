#include "canvas.h"

#include "core/input/inputController.h"

#include "common/common.h"

#include "core/gpu/gl/glUtil.h"
#include "core/gpu/gl/extraGl.h"
#include "core/system/io.h"

#include <tvgGlRenderTarget.h>
#include <tvgCanvas.h>

namespace core
{

CanvasWrapper::CanvasWrapper(void* context, Size size, bool bIsSw) : rContext(context), mIsSw(bIsSw)
{
	mRenderTarget = new GlRenderTarget();

	if (mIsSw)
	{
		mCanvas = tvg::SwCanvas::gen();
	}
	else
	{
		mCanvas = tvg::GlCanvas::gen();
	}
	resize(size);
}
CanvasWrapper::~CanvasWrapper()
{
}

void CanvasWrapper::onUpdate()
{
	if (mBeforeSize != mSize)
	{
		resize(mSize);
		onResize();
	}
	mGlobalElapsed += static_cast<uint32_t>(io::deltaTime * 1000.0);

	for (auto& anim : mAnimations)
	{
		anim->frame(mGlobalElapsed);
	}
}

void CanvasWrapper::draw()
{
	if (mIsDirty == false)
	{
		return;
	}
	mIsDirty = false;
	mCanvas->update();
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRenderTarget->getResolveFboId());
		glViewport(0, 0, (int) mSize.x, (int) mSize.y);
		glClearColor(mClearColor[0], mClearColor[1], mClearColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	mCanvas->draw(mIsSw);
	mCanvas->sync();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (mIsSw && mSwBuffer)
	{
		int stride = static_cast<int>(mSize.x);
		int height = static_cast<int>(mSize.y);
		int width = static_cast<int>(mSize.x);

		// todo: cache buffer
		uint32_t* tempBuffer = new uint32_t[width * height];
		for (int y = 0; y < height; ++y)
		{
			uint32_t* srcRow = mSwBuffer + y * stride;
			uint32_t* dstRow = tempBuffer + (height - 1 - y) * stride;

			std::memcpy(dstRow, srcRow, sizeof(uint32_t) * width);
		}
		glBindTexture(GL_TEXTURE_2D, getTexture());

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempBuffer);
		delete[] tempBuffer;
	}
}

void CanvasWrapper::resize(Size size)
{
	mBeforeSize = mSize;
	mSize = size;
	int32_t stride = (int32_t) size.w;
	int32_t w = (int32_t) size.w;
	int32_t h = (int32_t) size.h;

	mRenderTarget->reset();
	mRenderTarget->setViewport(tvg::RenderRegion{.min = {0, 0}, .max = {w, h}});
	mRenderTarget->init(w, h, 0);

	mCanvas->sync();
	if (mIsSw)
	{
		if (mSwBuffer)
			delete[] mSwBuffer;
		mSwBuffer = new uint32_t[w * h];
		static_cast<SwCanvas*>(mCanvas)->target(mSwBuffer, stride, w, h, tvg::ColorSpace::ABGR8888S);
	}
	else
	{
		static_cast<GlCanvas*>(mCanvas)->target(rContext, mRenderTarget->getResolveFboId(), w, h,
												tvg::ColorSpace::ABGR8888S);
	}
	mIsDirty = true;
}

void CanvasWrapper::update()
{
	mCanvas->update();
}

void CanvasWrapper::setDirty(bool dirty)
{
	mIsDirty = dirty;
}

uint32_t CanvasWrapper::getTexture()
{
	return mRenderTarget->getColorTexture();
}

unsigned char* CanvasWrapper::getBuffer()
{
	if (mBuffer != nullptr)
	{
		delete[] mBuffer;
		mBuffer = nullptr;
	}

	mBuffer =
		gl::util::ToBuffer(mRenderTarget->getResolveFboId(), static_cast<int>(mSize.x), static_cast<int>(mSize.y));

	return mBuffer;
}

InputController* CanvasWrapper::getInputController()
{
	return nullptr;
}

void CanvasWrapper::moveCamera(Vec2 xy)
{
}

}	 // namespace core