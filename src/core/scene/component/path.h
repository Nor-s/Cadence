#ifndef __CORE_ENTITY_PATH_H_
#define __CORE_ENTITY_PATH_H_

#include "common/common.h"
#include "../entity.h"
#include "keyframe.h"

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <algorithm>

namespace core
{
struct ShapeComponent;
struct IPath
{
	enum class Type
	{
		Rect,
		Ellipse,
		Path,
		Polygon,
		Star
	};
	IPath();
	virtual ~IPath();
	virtual Type type() = 0;
	virtual bool update(float frameNo) = 0;
	virtual void appendTo(ShapeComponent& s) = 0;
};

struct VisibleComponent
{
	bool isVisible = true;
};

struct PathListComponent
{
	std::vector<std::unique_ptr<IPath>> paths;

	PathListComponent();
	~PathListComponent();

	PathListComponent(const PathListComponent&) = delete;
	PathListComponent& operator=(const PathListComponent&) = delete;
	PathListComponent(PathListComponent&&) noexcept = default;
	PathListComponent& operator=(PathListComponent&&) noexcept = default;
};

// shape
struct RectPath : public IPath
{
	float radius{0.0};
	Vec2 position{0.0f, 0.0f};
	Vec2 scale{100.0f, 100.0f};

	FloatKeyFrame radiusKeyframes;
	VectorKeyFrame positionKeyframes;
	VectorKeyFrame scaleKeyframes;

	RectPath() = default;
	~RectPath() = default;

	Type type() override
	{
		return Type::Rect;
	}
	bool update(float frameNo) override
	{
		bool changed = false;
		UPDATE_KEYFRAME(radiusKeyframes, radius, frameNo, changed);
		UPDATE_KEYFRAME(positionKeyframes, position, frameNo, changed);
		UPDATE_KEYFRAME(scaleKeyframes, scale, frameNo, changed);
		return changed;
	}
	void appendTo(ShapeComponent& s) override;
};

struct EllipsePath : public IPath
{
	Vec2 position{0.0f, 0.0f};
	Vec2 scale{100.0f, 100.0f};

	VectorKeyFrame positionKeyframes;
	VectorKeyFrame scaleKeyframes;

	EllipsePath() = default;
	~EllipsePath() = default;

	Type type() override
	{
		return Type::Ellipse;
	}
	bool update(float frameNo) override
	{
		bool changed = false;
		UPDATE_KEYFRAME(positionKeyframes, position, frameNo, changed);
		UPDATE_KEYFRAME(scaleKeyframes, scale, frameNo, changed);
		return changed;
	}
	void appendTo(ShapeComponent& s) override;
};

struct RawPath : public IPath
{
	PathPoints path{};
	Vec2 center{};
	RawPath() = default;
	~RawPath() = default;
	Type type() override
	{
		return Type::Path;
	}
	bool update(float frameNo) override
	{
		bool changed = false;
		for (auto& p : path)
		{
			changed |= p.update(frameNo);
		}
		return changed;
	}
	void appendTo(ShapeComponent& s) override;
};

struct PolygonPath : public IPath
{
	int points{3};
	float rotation{0.0f};
	float outerRadius{100.0f};
	Vec2 position{0.0f, 0.0f};

	RawPath path;

	IntegerKeyFrame pointsKeyframes;
	FloatKeyFrame rotationKeyframes;
	FloatKeyFrame outerRadiusKeyframes;
	VectorKeyFrame positionKeyframes;

	PolygonPath() = default;
	~PolygonPath() = default;

	Type type() override
	{
		return Type::Polygon;
	}
	bool update(float frameNo) override
	{
		bool changed = false;
		UPDATE_KEYFRAME(pointsKeyframes, points, frameNo, changed);
		UPDATE_KEYFRAME(rotationKeyframes, rotation, frameNo, changed);
		UPDATE_KEYFRAME(outerRadiusKeyframes, outerRadius, frameNo, changed);
		UPDATE_KEYFRAME(positionKeyframes, position, frameNo, changed);
		return changed;
	}
	void appendTo(ShapeComponent& s) override;
};

struct StarPolygonPath : public IPath
{
	int points{5};
	float rotation{0.0f};
	float outerRadius{100.0f};
	float innerRadius{100.0f};
	Vec2 position{0.0f, 0.0f};

	RawPath path;

	IntegerKeyFrame pointsKeyframes;
	FloatKeyFrame rotationKeyframes;
	FloatKeyFrame outerRadiusKeyframes;
	FloatKeyFrame innerRadiusKeyframes;
	VectorKeyFrame positionKeyframes;

	StarPolygonPath() = default;
	~StarPolygonPath() = default;

	Type type() override
	{
		return Type::Star;
	}
	bool update(float frameNo) override
	{
		bool changed = false;
		UPDATE_KEYFRAME(pointsKeyframes, points, frameNo, changed);
		UPDATE_KEYFRAME(rotationKeyframes, rotation, frameNo, changed);
		UPDATE_KEYFRAME(outerRadiusKeyframes, outerRadius, frameNo, changed);
		UPDATE_KEYFRAME(innerRadiusKeyframes, innerRadius, frameNo, changed);
		UPDATE_KEYFRAME(positionKeyframes, position, frameNo, changed);
		return changed;
	}
	void appendTo(ShapeComponent& s) override;
};

template <typename T>
struct PathTag;
template <>
struct PathTag<RectPath>
{
	static constexpr IPath::Type type = IPath::Type::Rect;
};
template <>
struct PathTag<EllipsePath>
{
	static constexpr IPath::Type type = IPath::Type::Ellipse;
};
template <>
struct PathTag<PolygonPath>
{
	static constexpr IPath::Type type = IPath::Type::Polygon;
};
template <>
struct PathTag<StarPolygonPath>
{
	static constexpr IPath::Type type = IPath::Type::Star;
};
template <>
struct PathTag<RawPath>
{
	static constexpr IPath::Type type = IPath::Type::Path;
};

}	 // namespace core

#endif