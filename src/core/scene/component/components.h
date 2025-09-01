#ifndef __CORE_ENTITY_COMPONENTS_H_
#define __CORE_ENTITY_COMPONENTS_H_

#include "common/common.h"
#include "../entity.h"

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <algorithm>

#include <thorvg.h>

namespace core
{

class GlGeometry;
class Scene;
class BBox;

// class Animator;
struct NameComponent
{
	std::string name{};

	NameComponent(std::string_view name) : name(name)
	{
	}
};
struct IDComponent
{
	EntityID id;
};
struct RelationshipComponent
{
	EntityID parentId{0};
	std::vector<EntityID> children;

	RelationshipComponent() = default;
	RelationshipComponent(EntityID parent) : parentId(parent)
	{
	}
};
struct ShapeComponent
{
	Entity owner;
	tvg::Shape* shape{nullptr};
};

template <typename T>
struct Keyframes
{
	struct Keyframe
	{
		uint32_t frame{0};
		T value{0};
		bool operator<(const Keyframe& rhs) const
		{
			return frame < rhs.frame;
		}
	};
	bool isEnable{false};
	std::vector<Keyframe> frames;
	T currentValue{};

	auto begin()
	{
		return frames.begin();
	}
	auto end()
	{
		return frames.end();
	}

	void add(uint32_t frameNo, const T& value)
	{
		auto it = std::find_if(frames.begin(), frames.end(),
							   [frameNo](const auto& keyframe) { return frameNo == keyframe.frame; });
		if (it == frames.end())
		{
			isEnable = true;
			if (frames.empty() && frameNo != 0)
			{
				frames.push_back(Keyframe{.frame = 0, .value = value});
			}
			frames.push_back(Keyframe{.frame = frameNo, .value = value});
			std::sort(frames.begin(), frames.end());
		}
		else
		{
			it->value = value;
		}
	}

	T frame(float frameNo)
	{
		if (!isEnable || frames.empty())
			return currentValue;
		if (frames.size() == 1)
			return currentValue = frames[0].value;

		auto it = std::lower_bound(frames.begin(), frames.end(), frameNo,
								   [](const Keyframe& k, float f) { return k.frame < f; });

		if (it == frames.end())
		{
			return frames.back().value;
		}
		if (frames.begin() == it)
		{
			return it->value;
		}

		const auto& lo = *(it - 1);
		const auto& hi = *it;
		const float denom = float(hi.frame - lo.frame);
		const float t = denom > 0.f ? (frameNo - static_cast<float>(lo.frame)) / denom : 0.f;
		assert(t >= 0.0f && t <= 1.0f);

		// todo: curve
		return currentValue = lerp(lo.value, hi.value, t);
	}
};

using IntegerKeyFrame = Keyframes<int>;
using FloatKeyFrame = Keyframes<float>;
using VectorKeyFrame = Keyframes<Vec2>;
using ColorKeyFrame = Keyframes<Vec3>;

struct SceneComponent
{
	Scene* scene{nullptr};
};

// shape
struct RectPathComponent
{
	float radius{0.0};
	Vec2 position{0.0f, 0.0f};
	Vec2 scale{100.0f, 100.0f};
	FloatKeyFrame radiusKeyframes;
	VectorKeyFrame positionKeyframes;
	VectorKeyFrame scaleKeyframes;
};

struct ElipsePathComponent
{
	Vec2 position{0.0f, 0.0f};
	Vec2 scale{100.0f, 100.0f};
	VectorKeyFrame positionKeyframes;
	VectorKeyFrame scaleKeyframes;
};

struct PathComponent
{
	PathPoints path{};
	Vec2 center{};
};

struct PolygonPathComponent
{
	int points{3};
	float rotation{0.0f};
	float outerRadius{100.0f};
	Vec2 position{0.0f, 0.0f};

	PathComponent path;

	IntegerKeyFrame pointsKeyframes;
	FloatKeyFrame rotationKeyframes;
	FloatKeyFrame outerRadiusKeyframes;
	VectorKeyFrame positionKeyframes;
};

struct StarPolygonPathComponent
{
	int points{5};
	float rotation{0.0f};
	float outerRadius{100.0f};
	float innerRadius{100.0f};
	Vec2 position{0.0f, 0.0f};

	PathComponent path;

	IntegerKeyFrame pointsKeyframes;
	FloatKeyFrame rotationKeyframes;
	FloatKeyFrame outerRadiusKeyframes;
	FloatKeyFrame innerRadiusKeyframes;
	VectorKeyFrame positionKeyframes;
};

struct TransformComponent
{
	Vec2 anchorPoint{0.0f, 0.0f};			 // local anchor
	Vec2 worldPosition{0.0f, 0.0f};			 // world position
	Vec2 localCenterPosition{0.0f, 0.0f};	 // center of layer
	Vec2 scale{1.0f, 1.0f};
	float rotation{};
	tvg::Matrix transform;

	// todo: parent transform
	void update()
	{
		transform = identity();
		applyTranslate(&transform, localCenterPosition);
		applyRotateR(&transform, rotation);
		applyScaleR(&transform, scale);
		applyTranslateR(&transform, anchorPoint * -1.0f);
		worldPosition = localCenterPosition + anchorPoint;
	}
	tvg::Matrix inverse() const
	{
		auto ret = identity();
		return inverse(&transform, &ret) ? ret : identity();
	}
	static inline void identity(tvg::Matrix* m)
	{
		m->e11 = 1.0f;
		m->e12 = 0.0f;
		m->e13 = 0.0f;
		m->e21 = 0.0f;
		m->e22 = 1.0f;
		m->e23 = 0.0f;
		m->e31 = 0.0f;
		m->e32 = 0.0f;
		m->e33 = 1.0f;
	}
	static inline constexpr const tvg::Matrix identity()
	{
		return {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	}
	static inline void applyTranslate(tvg::Matrix* m, const Vec2& p)
	{
		m->e13 += p.x;
		m->e23 += p.y;
	}
	static inline void applyScaleR(tvg::Matrix* m, const Vec2& p)
	{
		m->e11 *= p.x;
		m->e21 *= p.x;
		m->e12 *= p.y;
		m->e22 *= p.y;
	}
	static inline void applyTranslateR(tvg::Matrix* m, const Vec2& p)
	{
		if (p.x == 0.0f && p.y == 0.0f)
			return;
		m->e13 += (p.x * m->e11 + p.y * m->e12);
		m->e23 += (p.x * m->e21 + p.y * m->e22);
	}
	static void applyRotateR(tvg::Matrix* m, float degree)
	{
		if (degree == 0.0f)
			return;

		const float rad = degree * float(KPI) / 180.0f;
		const float c = cosf(rad);
		const float s = sinf(rad);

		const float e11 = m->e11, e12 = m->e12;
		const float e21 = m->e21, e22 = m->e22;

		m->e11 = c * e11 + s * e12;
		m->e21 = c * e21 + s * e22;
		m->e12 = -s * e11 + c * e12;
		m->e22 = -s * e21 + c * e22;
	}

	static bool inverse(const tvg::Matrix* m, tvg::Matrix* out)
	{
		auto det = m->e11 * (m->e22 * m->e33 - m->e32 * m->e23) - m->e12 * (m->e21 * m->e33 - m->e23 * m->e31) +
				   m->e13 * (m->e21 * m->e32 - m->e22 * m->e31);

		auto invDet = 1.0f / det;
		if (std::isinf(invDet))
			return false;

		out->e11 = (m->e22 * m->e33 - m->e32 * m->e23) * invDet;
		out->e12 = (m->e13 * m->e32 - m->e12 * m->e33) * invDet;
		out->e13 = (m->e12 * m->e23 - m->e13 * m->e22) * invDet;
		out->e21 = (m->e23 * m->e31 - m->e21 * m->e33) * invDet;
		out->e22 = (m->e11 * m->e33 - m->e13 * m->e31) * invDet;
		out->e23 = (m->e21 * m->e13 - m->e11 * m->e23) * invDet;
		out->e31 = (m->e21 * m->e32 - m->e31 * m->e22) * invDet;
		out->e32 = (m->e31 * m->e12 - m->e11 * m->e32) * invDet;
		out->e33 = (m->e11 * m->e22 - m->e21 * m->e12) * invDet;

		return true;
	}
};
inline static void operator*=(Vec2& pt, const tvg::Matrix& m)
{
	auto tx = pt.x * m.e11 + pt.y * m.e12 + m.e13;
	auto ty = pt.x * m.e21 + pt.y * m.e22 + m.e23;
	pt.x = tx;
	pt.y = ty;
}

inline static Vec2 operator*(const Vec2& pt, const tvg::Matrix& m)
{
	auto tx = pt.x * m.e11 + pt.y * m.e12 + m.e13;
	auto ty = pt.x * m.e21 + pt.y * m.e22 + m.e23;
	return {tx, ty};
}

struct TransformKeyframeComponent
{
	VectorKeyFrame positionKeyframes;
	VectorKeyFrame scaleKeyframes;
	FloatKeyFrame rotationKeyframes;
};

struct SolidFillComponent
{
	Vec3 color = CommonSetting::Color_DefaultFill;
	float alpha{255.0f};
	tvg::FillRule rule{tvg::FillRule::NonZero};
	ColorKeyFrame colorKeyframe;
	FloatKeyFrame alphaKeyframe;
};

struct StrokeComponent
{
	Vec3 color = CommonSetting::Color_DefaultStroke;
	float alpha{255.0f};
	float width{1.5f};
	// tvg::StrokeJoin join;
	ColorKeyFrame colorKeyframe;
	FloatKeyFrame widthKeyframe;
	FloatKeyFrame alphaKeyframe;
};

}	 // namespace core

// update component
namespace core
{
static void Reset(ShapeComponent& shape)
{
	shape.shape->reset();
}

static void Update(ShapeComponent& shape, TransformComponent& transform)
{
	transform.update();
	shape.shape->transform(transform.transform);
}

static void Update(ShapeComponent& shape, PathComponent& path)
{
	Reset(shape);
	if (!path.path.empty())
	{
		std::vector<tvg::PathCommand> types;
		std::vector<tvg::Point> points;

		for (int i = 0; i < path.path.size(); i++)
		{
			auto& p = path.path.at(i);
			switch (p.type)
			{
				case PathPoint::Command::Close:
				{
					types.emplace_back(tvg::PathCommand::Close);
					break;
				}
				case PathPoint::Command::MoveTo:
				{
					types.emplace_back(tvg::PathCommand::MoveTo);
					points.push_back(tvg::Point{p.base.x, p.base.y});
					break;
				}
				case PathPoint::Command::CubicTo:
				{
					assert(i != 0);

					auto& left = path.path[i - 1];
					auto leftP = left.base + left.deltaRightControl;
					auto rightP = p.base + p.deltaLeftControl;

					types.emplace_back(tvg::PathCommand::CubicTo);
					points.push_back(tvg::Point{leftP.x, leftP.y});
					points.push_back(tvg::Point{rightP.x, rightP.y});
					points.push_back(tvg::Point{p.base.x, p.base.y});
					break;
				}
				case PathPoint::Command::LineTo:
				{
					types.emplace_back(tvg::PathCommand::LineTo);
					points.push_back(tvg::Point{p.base.x, p.base.y});
					break;
				}
			}
		}
		shape.shape->appendPath(types.data(), types.size(), points.data(), points.size());
	}
}

static void Update(ShapeComponent& shape, StarPolygonPathComponent& path)
{
	const float start = -90.0f;
	const float step = 360.0f / (path.points * 2.0f);
	const int iterCount = 2 * path.points;

	path.path.path.clear();

	for (int i = 0; i < iterCount; ++i)
	{
		const bool isOuter = (i % 2 == 0);
		const float r = isOuter ? path.outerRadius : path.innerRadius;
		const float degree = start + i * step;

		const float px = std::cos(ToRadian(degree)) * r;
		const float py = std::sin(ToRadian(degree)) * r;

		auto type = (i == 0 ? PathPoint::Command::MoveTo : PathPoint::Command::LineTo);
		path.path.path.push_back({.base{px, py}, .type = type});
	}
	path.path.path.push_back({.type = PathPoint::Command::Close});

	Update(shape, path.path);
}

static void Update(ShapeComponent& shape, PolygonPathComponent& path)
{
	const float start = -90.0f;
	const float step = 360.0f / path.points;

	path.path.path.clear();

	for (int i = 0; i < path.points; ++i)
	{
		const bool isOuter = (i % 2 == 0);
		const float r = path.outerRadius;
		const float degree = start + i * step;

		const float px = std::cos(ToRadian(degree)) * r;
		const float py = std::sin(ToRadian(degree)) * r;
		auto type = (i == 0 ? PathPoint::Command::MoveTo : PathPoint::Command::LineTo);

		path.path.path.push_back({.base{px, py}, .type = type});
	}

	path.path.path.push_back({.type = PathPoint::Command::Close});

	Update(shape, path.path);
}

static void Update(ShapeComponent& shape, ElipsePathComponent& path)
{
	Reset(shape);
	shape.shape->appendCircle(path.position.x, path.position.y, path.scale.x * 0.5f, path.scale.y * 0.5f);
}

static void Update(ShapeComponent& shape, RectPathComponent& path)
{
	Reset(shape);
	const float x = path.position.x - path.scale.x * 0.5f;
	const float y = path.position.y - path.scale.y * 0.5f;
	shape.shape->appendRect(x, y, path.scale.x, path.scale.y, path.radius, path.radius);
}
// shape.shape->reset();

static void Update(ShapeComponent& shape, SolidFillComponent& fill)
{
	shape.shape->fill(static_cast<uint8_t>(fill.color.x), static_cast<uint8_t>(fill.color.y),
					  static_cast<uint8_t>(fill.color.z), static_cast<uint8_t>(fill.alpha));
	shape.shape->fillRule(fill.rule);
}

static void Update(ShapeComponent& shape, StrokeComponent& stroke)
{
	shape.shape->strokeWidth(stroke.width);
	shape.shape->strokeFill(static_cast<uint32_t>(stroke.color.x), static_cast<uint32_t>(stroke.color.y),
							static_cast<uint32_t>(stroke.color.z), static_cast<uint32_t>(stroke.alpha));
}

template <typename TComponent>
static bool UpdateShape(Entity& entity, ShapeComponent& shape)
{
	if (entity.hasComponent<TComponent>())
	{
		auto& component = entity.getComponent<TComponent>();
		Update(shape, component);
		return true;
	}
	return false;
}

}	 // namespace core

#endif