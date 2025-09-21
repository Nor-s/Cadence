#ifndef __CORE_ENTITY_KEYFRAME_H_
#define __CORE_ENTITY_KEYFRAME_H_

#include "common/common.h"
#include "../entity.h"

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <algorithm>

#define UPDATE_KEYFRAME(KF, DST, FRAME, CHANGED) \
	if ((KF).isEnable)                           \
	{                                            \
		auto _v = (KF).frame(FRAME);             \
		if (_v != (DST))                         \
		{                                        \
			(DST) = _v;                          \
			(CHANGED) = true;                    \
		}                                        \
	}

namespace core
{

inline float cubicBezierProgress(const Vec2& p1, const Vec2& p2, float t)
{
	t = std::clamp(t, 0.0f, 1.0f);

	auto bezier = [](float s, float p0, float p1, float p2, float p3)
	{
		float u = 1.0f - s;
		return u * u * u * p0 + 3.0f * u * u * s * p1 + 3.0f * u * s * s * p2 + s * s * s * p3;
	};

	float lo = 0.0f, hi = 1.0f, s = 0.5f;
	const float eps = 1e-4f;
	for (int i = 0; i < 256; ++i)
	{
		s = 0.5f * (lo + hi);
		float x = bezier(s, 0.0f, p1.x, p2.x, 1.0f);
		if (fabsf(x - t) < eps)
			break;
		(x < t) ? lo = s : hi = s;
	}

	return bezier(s, 0.0f, p1.y, p2.y, 1.0f);
}

template <typename T>
struct Keyframes
{
	struct Keyframe
	{
		uint32_t frame{0};
		T value{0};
		Vec2 inTangent{0, 0};
		Vec2 outTangent{0, 0};

		bool operator<(const Keyframe& rhs) const
		{
			return frame < rhs.frame;
		}
	};
	bool isEnable{false};
	std::vector<Keyframe> frames;

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
	Keyframe* left(float frameNo)
	{
		if (frames.empty())
			return nullptr;

		auto it = std::lower_bound(frames.begin(), frames.end(), frameNo,
								   [](const Keyframe& k, float f) { return k.frame < f; });
		if (it == frames.end() || it == frames.begin())
		{
			return &*frames.begin();
		}
		return &*(it - 1);
	}
	Keyframe* right(float frameNo)
	{
		if (frames.empty())
			return nullptr;
		auto it = std::lower_bound(frames.begin(), frames.end(), frameNo,
								   [](const Keyframe& k, float f) { return k.frame < f; });
		if (it == frames.end())
		{
			return nullptr;
		}
		return &*it;
	}

	T frame(float frameNo)
	{
		if (!isEnable || frames.empty())
		{
			assert(false);
			return T{};
		}
		if (frames.size() == 1)
			return frames[0].value;

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
		const bool linear = (fabsf(lo.outTangent.x) < 1e-6f && fabsf(lo.outTangent.y) < 1e-6f &&
							 fabsf(hi.inTangent.x) < 1e-6f && fabsf(hi.inTangent.y) < 1e-6f);

		assert(t >= 0.0f && t <= 1.0f);
		float u = t;
		if (!linear)
		{
			u = cubicBezierProgress(lo.outTangent, hi.inTangent, t);
		}

		return lerp(lo.value, hi.value, u);
	}
};

using IntegerKeyFrame = Keyframes<int>;
using FloatKeyFrame = Keyframes<float>;
using VectorKeyFrame = Keyframes<Vec2>;
using ColorKeyFrame = Keyframes<Vec3>;

struct PathPoint
{
	enum class Command : int
	{
		LineTo = 0,
		MoveTo = 1,
		CubicTo = 2,
		Close =3
	};

	Vec2 localPosition{0.0f, 0.0f};
	Vec2 deltaLeftControlPosition{0.0f, 0.0f};
	Vec2 deltaRightControlPosition{0.0f, 0.0f};
	Command type{Command::LineTo};

	VectorKeyFrame localPositionKeyframe;
	VectorKeyFrame deltaLeftControlPositionKeyframe;
	VectorKeyFrame deltaRightControlPositionKeyframe;

	bool update(float frameNo)
	{
		bool changed = false;

		UPDATE_KEYFRAME(localPositionKeyframe, localPosition, frameNo, changed);
		if (type != Command::LineTo)
		{
			UPDATE_KEYFRAME(deltaLeftControlPositionKeyframe, deltaLeftControlPosition, frameNo, changed);
			UPDATE_KEYFRAME(deltaRightControlPositionKeyframe, deltaRightControlPosition, frameNo, changed);
		}

		return changed;
	}
};

using PathPoints = std::vector<PathPoint>;

}	 // namespace core

#endif