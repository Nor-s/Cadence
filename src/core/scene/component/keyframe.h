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
		assert(t >= 0.0f && t <= 1.0f);

		// todo: curve
		return lerp(lo.value, hi.value, t);
	}
};

using IntegerKeyFrame = Keyframes<int>;
using FloatKeyFrame = Keyframes<float>;
using VectorKeyFrame = Keyframes<Vec2>;
using ColorKeyFrame = Keyframes<Vec3>;

}	 // namespace core

#endif