#ifndef _CORE_CANVAS_SCENE_UI_CONTROL_BOX_H_
#define _CORE_CANVAS_SCENE_UI_CONTROL_BOX_H_

#include <memory>
#include "scene/scene.h"

namespace core
{

class IFunction
{
public:
	virtual bool invoke() = 0;
};

template <typename Func>
class LambdaWrapper : public IFunction
{
public:
	LambdaWrapper(Func func) : mFunc(func)
	{
	}
	virtual bool invoke() override
	{
		return mFunc();
	}

private:
	Func mFunc;
};

template <typename TargetClass, typename MemberFunc>
class MemberFunctionWrapper : public IFunction
{
public:
	MemberFunctionWrapper(TargetClass* target, MemberFunc func) : mFunc(func)
	{
	}
	virtual bool invoke() override
	{
		return (rClassPointer->*mFunc)();
	}

private:
	TargetClass* rClassPointer;
	MemberFunc mFunc;
};

template <typename TargetClass, typename F>
auto MakeMemberFunction(TargetClass* target, F&& f)
{
	using FnType = std::decay_t<F>;
	return std::make_unique<MemberFunctionWrapper<TargetClass, FnType>>(target, std::forward<F>(f));
}

template <typename F>
auto MakeLambda(F&& f)
{
	using FnType = std::decay_t<F>;
	return std::make_unique<LambdaWrapper<FnType>>(std::forward<F>(f));
}

class UIShape
{
public:
	// for cursor shape
	enum class CursorType
	{
		Move,
		Rotate,
		Scale,
		Click,
		None
	};

	enum class ShapeType
	{
		Rect,
		Ellipse,
		Path,
		Polygon
	};

	enum class ShapeAttribute : uint8_t
	{
		Transparent = 0b00,
		Stroke = 0b01,
		Fill = 0b10,
		FillStroke = 0b11,
	};

	enum class State
	{
		None,
		Normal,
		Hovered,
		Pressed
	};

	struct StateAttribute
	{
		// -1 is default setting
		Vec3 fillColor;
		float fillAlpha;
		Vec3 strokeColor;
		float strokeAlpha;
		float strokeWidth ;
		// Vec2 scale{1.0f, 1.0f};
		StateAttribute() : fillColor(-1.0f, -1.0f, -1.0f), fillAlpha(-1.0f), strokeColor(-1.0f, -1.0f, -1.0f),
						   strokeAlpha(-1.0f), strokeWidth(-1.0f)
		{
		}
	};

	struct Attribute
	{
		CursorType cursorType;
		ShapeType shapeType ;
		ShapeAttribute shapeAtt ;

		float animationTime;
		StateAttribute normal;
		StateAttribute hovered;
		StateAttribute pressed;
		Attribute()
			: cursorType(CursorType::Click), shapeType(ShapeType::Rect), shapeAtt(ShapeAttribute::FillStroke), 
			  animationTime(0.1f), normal(), hovered(), pressed()
		{
		}
	};

public:
	// todo: add input controller
	UIShape(Scene* scene, Vec2 center, float w, const Attribute& attribute = Attribute());
	UIShape(Scene* scene, Vec2 center, Vec2 wh, const Attribute& attribute = Attribute());
	UIShape(Scene* scene, PathPoint start, PathPoint end, const Attribute& attribute = Attribute());
	UIShape(Scene* scene, const std::array<Vec2, 4>& obbPoints, const Attribute& attribute = Attribute());
	~UIShape();

	void moveTo(const Vec2& xy);
	void moveByDelta(const Vec2& xy);
	bool onStartLeftDown(Vec2 xy);

	bool onDragLeftMouse();
	bool onEndLeftMouse();
	bool onMoveMouse(const Vec2& xy);
	bool onUpdate()
	{
		if (mOnUpdate)
		{
			return mOnUpdate->invoke();
		}
		return false;
	}
	void updatePath(PathPoints pathPoints);

	Vec2 getWorldPosition();
	void setOnLeftDrag(std::unique_ptr<IFunction> onLeftDrag)
	{
		mOnLeftDrag = std::move(onLeftDrag);
	}
	void setOnUpdate(std::unique_ptr<IFunction> onUpdate)
	{
		mOnUpdate = std::move(onUpdate);
	}

	void setVisible(bool visible);
	bool isVisible() const;

private:
	void init();

private:
	Scene* rScene{nullptr};
	Entity mEntity;
	std::unique_ptr<IFunction> mOnLeftDrag;
	std::unique_ptr<IFunction> mOnUpdate;
	std::array<Vec2, 4> mObbPoints;
	State mState{State::Normal};
	Attribute mAtt;

	Vec3 mCurrentFillColor;
	float mCurrentFillAlpha;

	Vec3 mCurrentStrokeColor;
	float mCurrentStrokeAlpha;
};

}	 // namespace core

#endif