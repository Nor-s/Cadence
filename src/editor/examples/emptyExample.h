/*
 * Copyright (c) 2025 the ThorVG project. All rights reserved.

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

#ifndef _EDITOR_EXAMPLES_EMPTY_EXAMPLE_H_
#define _EDITOR_EXAMPLES_EMPTY_EXAMPLE_H_

#include "exampleBase.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct EmptyExample : tvgexam::Example
{
	std::string_view toString() override
	{
		static std::string str = "Empty example";
		return str;
	}

	bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
	{
		{
			// background
			auto shape = tvg::Shape::gen();
			shape->appendRect(0, 0, (float) w, (float) h);
			shape->fill(244, 244, 244);
			canvas->push(shape);
		}

		return true;
	}
};

#endif