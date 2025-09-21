#ifndef _NS_CORE_COMMON_H_
#define _NS_CORE_COMMON_H_

#include <string_view>
#include <memory>
#include <numeric>

#include "logger.h"
#include "timer.h"
#include "vec.h"

namespace core
{

using Size = Vec2;

struct CommonSetting
{
	inline static const Vec3 Color_DefaultHoverOutline{200.0f, 100.0f, 50.0f};
	inline static const Vec3 Color_DefaultControlBoxOutline{255.0f, 127.0f, 63.0f};
	inline static const Vec3 Color_DefaultControlBoxInner{250.0f, 250.0f, 255.0f};
	inline static const Vec3 Color_DefaultActiveFillStrokeEllipse{250.0f, 180.0f, 130.0f};
	inline static const Vec3 Color_DefaultStroke{4.0f, 5.0f, 5.0f};
	inline static const Vec3 Color_DefaultFill{127.0f, 127.0f, 127.0f};

	inline static const float Width_DefaultBBoxControlBox{10.0f};
	inline static const float Width_DefaultBBoxRotationControlBox{50.0f};
	inline static const float Width_DefaultPathPointControlBox{10.0f};
	inline static const float Width_DefaultPathLine{5.0f};
	inline static const float Width_DefaultStroke{1.5f};

	inline static int Count_DefaultPolygonPathPoint{3};
	inline static int Count_DefaultStarPolygonPathPoint{5};

	inline static const float Threshold_AddPathModeChangeCurve{200.0f};
	inline static const float Threshold_AddPathLayer{0.5f};
};

}	 // namespace core

#endif