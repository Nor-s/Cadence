#ifndef __CORE_INPUT_INPUT_H_
#define __CORE_INPUT_INPUT_H_

#include "common/common.h"

namespace core
{

enum InputType : uint8_t
{
	MOUSE_LEFT_DOWN = 0u,
	MOUSE_LEFT_DOUBLE_CLICK,
	MOUSE_MIDDLE_DOWN,
	MOUSE_MOVE,
	MOUSE_WHEEL,
	INPUT_TYPE_SIZE,
	INPUT_DETACH,
	INPUT_ATTACH,
};

enum class InputTrigger : uint8_t
{
	None = 0,
	Started = 1 << 1,
	Triggered = 1 << 2,
	Ended = 1 << 3,
};

inline InputTrigger operator|(InputTrigger a, InputTrigger b)
{
	return static_cast<InputTrigger>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline InputTrigger operator&(InputTrigger a, InputTrigger b)
{
	return static_cast<InputTrigger>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline bool HasFlag(InputTrigger value, InputTrigger flag)
{
	return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

class InputValue
{
public:
	InputValue(float x) : mValue(x, x, x)
	{
	}
	InputValue(float x, float y) : mValue(x, y, 0.0f)
	{
	}
	InputValue(float x, float y, float z) : mValue(x, y, z)
	{
	}
	InputValue(const Vec2& xy) : mValue(xy.x, xy.y, 0.0f)
	{
	}
	InputValue(const Vec3& xyz) : mValue(xyz)
	{
	}
	template <typename T>
	T get() const
	{
		return static_cast<T>(mValue);
	}
	template <typename T>
	T getDelta() const
	{
		return static_cast<T>(mValue - mBeforeValue);
	}

private:
	friend class InputController;
	Vec3 mValue;
	Vec3 mBeforeValue;
};

}	 // namespace core

#endif