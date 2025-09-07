#ifndef _CORE_CANVAS_SHAPE_UTIL_H_
#define _CORE_CANVAS_SHAPE_UTIL_H_

#include <thorvg.h>
#include <array>
#include <list>
#include "common/common.h"

#include "scene/scene.h"

namespace core
{

struct Line;
struct Segment;

float Evaluate(const Line& line, float x);
Line ToLine(const Segment& seg);

static std::array<Vec2, 4> GetObb(tvg::Paint* p)
{
	std::array<tvg::Point, 4> pts;
	std::array<Vec2, 4> ptsVec;
	p->bounds(pts.data());
	ptsVec[0] = Vec2{pts[0].x, pts[0].y};
	ptsVec[1] = Vec2{pts[1].x, pts[1].y};
	ptsVec[2] = Vec2{pts[2].x, pts[2].y};
	ptsVec[3] = Vec2{pts[3].x, pts[3].y};
	return ptsVec;
}
static bool IsInner(const std::array<Vec2, 4>& q, Vec2 p)
{
	const bool s1 = cross(q[0] - q[1], p - q[1]) >= 0.0f;
	const bool s2 = cross(q[1] - q[2], p - q[2]) >= 0.0f;
	const bool s3 = cross(q[2] - q[3], p - q[3]) >= 0.0f;
	const bool s4 = cross(q[3] - q[0], p - q[0]) >= 0.0f;
	return (s1 == s2) && (s2 == s3) && (s3 == s4);
}
static bool IsInner(tvg::Paint* p, Vec2 point)
{
	auto obb = GetObb(p);
	return IsInner(obb, point);
}
static Vec2 GetCenter(const std::array<Vec2, 4>& q)
{
	return (q[0] + q[1] + q[2] + q[3]) / 4.0f;
}

struct PickInfo
{
	std::unordered_set<uint32_t> excludeIds;
	core::Scene* currentSelectedScene{nullptr};
	tvg::Paint* currentSelectedPaint{nullptr};
};

static bool Pick(PickInfo& pickInfo, const Vec2& point, tvg::Paint* paint, int depth)
{
	if (!paint || pickInfo.excludeIds.find(paint->id) != pickInfo.excludeIds.end())
		return false;

	const bool isScene = (paint->type() == tvg::Type::Scene);
	const bool isCurrentSelected = pickInfo.currentSelectedPaint == paint;

	if (IsInner(GetObb(paint), point))
	{
		if (isScene)
		{
			tvg::Scene* rawScene = static_cast<tvg::Scene*>(paint);
			core::Scene* scene = Scene::FindScene(rawScene);
			if (scene == nullptr || !scene->mIsMainScene)
				return false;

			pickInfo.currentSelectedScene = scene;
			if (depth == 0 || isCurrentSelected)
			{
				bool isPicked = false;
				for (auto* p : rawScene->paints())
				{
					isPicked |= Pick(pickInfo, point, p, depth + 1);
					if (isPicked)
						return true;
				}
				return isCurrentSelected || isPicked;
			}
		}
		if (isCurrentSelected)
			return false;
		pickInfo.currentSelectedPaint = paint;
		return true;
	}
	return false;
}

template <typename T>
static bool Pick(T* canvasOrScene, PickInfo& pickInfo, const Vec2& point)
{
	auto& paints = canvasOrScene->paints();
	bool isPicked = false;
	for (auto& paint : paints)
	{
		isPicked |= Pick(pickInfo, point, paint, 0);
	}
	return isPicked;
}
struct Line
{
	float intercept{0};
	float slope{0};

	// y = slope * x + intercept
	const float evalute(float x) const
	{
		return Evaluate(*this, x);
	}
	void moveX(float x)
	{
		intercept = Evaluate(*this, -x);
	}
	void setSlopeByDegree(float d)
	{
		auto radian = ToRadian(d);
		auto c = cos(radian);
		if (std::abs(c) < 1e-6f)
		{
			slope = 0;
		}
		else
		{
			slope = sin(radian) / c;
		}
	}
	static Line Gen(const Vec2& point, float degree)
	{
		Line line;
		line.setSlopeByDegree(degree);
		line.intercept = point.y - line.slope * point.x;
		return line;
	}
};

struct Segment
{
	Vec2 start{};
	Vec2 end{};
};

inline static bool Intersect(const Line& source, const Line& target, Vec2& out)
{
	if (std::abs(source.slope - target.slope) < 1e-6)
		return false;
	out.x = (source.intercept - target.intercept) / (target.slope - source.slope);
	out.y = source.evalute(out.x);
	return true;
}

inline static bool Intersect(const Line& source, const Segment& target, Vec2& out)
{
	Line targetLine = ToLine(target);
	bool isIntersect = Intersect(source, targetLine, out);
	return (isIntersect && target.start.x <= out.x && out.x <= target.end.x);
}
inline static bool Intersect(const Segment& source, const Segment& target, Vec2& out)
{
	Line sourceLine = ToLine(source);
	bool isIntersect = Intersect(sourceLine, target, out);
	return (isIntersect && source.start.x <= out.x && out.x <= source.end.x);
}
inline static Vec2 Rotate(const Vec2& point, float degree)
{
	Vec2 ret;
	auto c = cos(ToRadian(degree));
	auto s = sin(ToRadian(degree));
	ret.x = point.x * c - point.y * s;
	ret.y = point.x * s + point.y * c;
	return ret;
}

}	 // namespace core

#endif