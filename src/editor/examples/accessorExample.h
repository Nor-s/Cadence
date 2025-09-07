#ifndef _EDITOR_EXAMPLES_ACCESSOR_EXAMPLE_H_
#define _EDITOR_EXAMPLES_ACCESSOR_EXAMPLE_H_

#include "exampleBase.h"

/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

struct AccessorExample : tvgexam::Example
{
	std::string_view toString() override
	{
		static std::string str = "accessor example";
		return str;
	}

	bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
	{
		// load the tvg file
		auto picture = tvg::Picture::gen();
		auto result = picture->load(RESOURCE_DIR "/svg/favorite_on.svg");
		if (!tvgexam::verify(result))
			return false;
		picture->size(static_cast<float>(w), static_cast<float>(h));

		auto accessor = unique_ptr<tvg::Accessor>(tvg::Accessor::gen());

		// The callback function from lambda expression.
		// This function will be called for every paint nodes of the picture tree.
		auto f = [](const tvg::Paint* paint, void* data) -> bool
		{
			if (paint->type() == tvg::Type::Shape)
			{
				auto shape = (tvg::Shape*) paint;
				// override color?
				uint8_t r, g, b;
				shape->fill(&r, &g, &b);
				if (r == 255 && g == 180 && b == 0)
					shape->fill(0, 0, 255);
			}

			// You can return false, to stop traversing immediately.
			return true;
		};

		if (!tvgexam::verify(accessor->set(picture, f, nullptr)))
			return false;

		// Try to retrieve the shape that corresponds to the SVG node with the unique ID "star".
		if (auto paint = picture->paint(tvg::Accessor::id("star")))
		{
			auto shape = static_cast<tvg::Shape*>(const_cast<tvg::Paint*>(paint));
			shape->strokeFill(255, 255, 0);
			shape->strokeWidth(5);
		}

		canvas->push(picture);

		return true;
	}
};

#endif