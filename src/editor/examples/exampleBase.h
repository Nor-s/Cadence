/*
 * Copyright (c) 2024 - 2025 the ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _EDITOR_EXAMPLES_EXAMPLE_BASE_H_
#define _EDITOR_EXAMPLES_EXAMPLE_BASE_H_

// #include "config.h"

#include <core/core.h>

#include <algorithm>
#include <memory>
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

#include <thorvg.h>

#ifdef _WIN32
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#else
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#endif

#include <SDL.h>

using namespace std;

namespace tvgexam
{

struct Example
{
	uint32_t elapsed = 0;

	virtual std::string_view toString() = 0;

	virtual bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) = 0;
	virtual bool update(tvg::Canvas* canvas, uint32_t elapsed)
	{
		return false;
	}
	virtual bool clickdown(tvg::Canvas* canvas, int32_t x, int32_t y)
	{
		return false;
	}
	virtual bool clickup(tvg::Canvas* canvas, int32_t x, int32_t y)
	{
		return false;
	}
	virtual bool motion(tvg::Canvas* canvas, int32_t x, int32_t y)
	{
		return false;
	}
	virtual void populate(const char* path)
	{
	}
	virtual ~Example()
	{
	}

	float timestamp()
	{
		return float(SDL_GetTicks()) * 0.001f;
	}

	void scandir(const char* path)
	{
		char buf[PATH_MAX];

		// real path
#ifdef _WIN32
		auto rpath = _fullpath(buf, path, PATH_MAX);
#else
		auto rpath = realpath(path, buf);
#endif

		// open directory
#ifdef _WIN32
		WIN32_FIND_DATA fd;
		HANDLE h =
			FindFirstFileEx((string(rpath) + "\\*").c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, 0);
		if (h == INVALID_HANDLE_VALUE)
		{
			cout << "Couldn't open directory \"" << rpath << "\"." << std::endl;
			return;
		}
		do
		{
			if (*fd.cFileName == '.' || *fd.cFileName == '$')
				continue;
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				auto fullpath = string(path);
				fullpath += '\\';
				fullpath += fd.cFileName;
				populate(fullpath.c_str());
			}
		} while (FindNextFile(h, &fd));
		FindClose(h);
#else
		DIR* dir = opendir(rpath);
		if (!dir)
		{
			std::cout << "Couldn't open directory \"" << rpath << "\"." << std::endl;
			return;
		}

		// list directory
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (*entry->d_name == '.' || *entry->d_name == '$')
				continue;
			if (entry->d_type != DT_DIR)
			{
				auto fullpath = std::string(path);
				fullpath += '/';
				fullpath += entry->d_name;
				populate(fullpath.c_str());
			}
		}
		closedir(dir);
#endif
	}
};

inline static float progress(uint32_t elapsed, float durationInSec, bool rewind = false)
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

inline static bool verify(tvg::Result result, std::string failMsg = "")
{
	switch (result)
	{
		case tvg::Result::FailedAllocation:
		{
			std::cout << "FailedAllocation! " << failMsg << std::endl;
			return false;
		}
		case tvg::Result::InsufficientCondition:
		{
			std::cout << "InsufficientCondition! " << failMsg << std::endl;
			return false;
		}
		case tvg::Result::InvalidArguments:
		{
			std::cout << "InvalidArguments! " << failMsg << std::endl;
			return false;
		}
		case tvg::Result::MemoryCorruption:
		{
			std::cout << "MemoryCorruption! " << failMsg << std::endl;
			return false;
		}
		case tvg::Result::NonSupport:
		{
			std::cout << "NonSupport! " << failMsg << std::endl;
			return false;
		}
		case tvg::Result::Unknown:
		{
			std::cout << "Unknown! " << failMsg << std::endl;
			return false;
		}
		default:
			break;
	};
	return true;
}

class ExampleCanvas : public core::CanvasWrapper
{
public:
	ExampleCanvas(void* context, core::Size size, bool bIsSw);

	void onInit() override
	{
		mCanvas->remove();
		mPaints.clear();
		mAnimations.clear();
		mExample->content(mCanvas, static_cast<uint32_t>(mSize.x), static_cast<uint32_t>(mSize.y));
		mExample->elapsed = 0u;
	}

	void onUpdate() override
	{
		if (gExampleList[mCurrentExampleIdx].get() != rExample)
		{
			rExample = gExampleList[mCurrentExampleIdx].get();
			mExample = gMakeExample[mCurrentExampleIdx]();
			onInit();
		}
		core::CanvasWrapper::onUpdate();

		// rExample->elapsed += static_cast<uint32_t>(core::io::deltaTime * 1000);
		mExample->update(mCanvas, mGlobalElapsed);
	}

	void onResize() override
	{
		mCanvas->remove();
		mExample->content(mCanvas, static_cast<uint32_t>(mSize.x), static_cast<uint32_t>(mSize.y));
		for (auto& paint : mPaints)
		{
			paint->scale(mSize);
			mCanvas->push(paint->mHandle);
		}
	}

	core::CanvasType type() override
	{
		return core::CanvasType::Example;
	}

	static inline std::vector<std::unique_ptr<Example>> gExampleList;
	static inline std::vector<std::function<std::unique_ptr<Example>()>> gMakeExample;
	uint32_t mCurrentExampleIdx = 0;

private:
	Example* rExample = nullptr;
	std::unique_ptr<Example> mExample = nullptr;
};

}	 // namespace tvgexam

#endif