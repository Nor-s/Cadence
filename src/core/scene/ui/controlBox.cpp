#include "controlBox.h"

#include "common/common.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"

#include "canvas/shapeUtil.h"

namespace core
{

UIShape::UIShape(Scene* scene, Vec2 center, Vec2 wh, const Attribute& att) : mAtt(att)
{
	rScene = scene;
	switch (mAtt.shapeType)
	{
		case ShapeType::Polygon:
		{
			mEntity = rScene->createPolygonFillStrokeLayer(center - wh / 2.0f, wh);
			break;
		}
		case ShapeType::Rect:
		{
			mEntity = rScene->createRectFillStrokeLayer(center - wh / 2.0f, wh);
			break;
		}
		case ShapeType::Ellipse:
		{
			mEntity = rScene->createEllipseFillStrokeLayer(center - wh / 2.0f, wh);
			break;
		}
		case ShapeType::Path:
		{
			PathPoints pathPoints;
			auto min = center - wh;
			pathPoints.push_back(PathPoint{.localPosition = min, .type = PathPoint::Command::MoveTo});
			pathPoints.push_back(
				PathPoint{.localPosition = min + Vec2{wh.w, 0.0f}, .type = PathPoint::Command::LineTo});
			pathPoints.push_back(
				PathPoint{.localPosition = min + Vec2{wh.w, wh.h}, .type = PathPoint::Command::LineTo});
			pathPoints.push_back(
				PathPoint{.localPosition = min + Vec2{0.0f, wh.h}, .type = PathPoint::Command::LineTo});
			pathPoints.push_back(PathPoint{.type = PathPoint::Command::Close});
			mEntity = rScene->createPathLayer(pathPoints);

			break;
		}
	}
	if (((uint8_t) mAtt.shapeAtt & (uint8_t) ShapeAttribute::Stroke) == 0)
	{
		mAtt.normal.strokeAlpha = 0.0f;
		mAtt.pressed.strokeAlpha = 0.0f;
		mAtt.hovered.strokeAlpha = 0.0f;
	}
	if (((uint8_t) mAtt.shapeAtt & (uint8_t) ShapeAttribute::Fill) == 0)
	{
		mAtt.normal.fillAlpha = 0.0f;
		mAtt.pressed.fillAlpha = 0.0f;
		mAtt.hovered.fillAlpha = 0.0f;
	}
	init();
}

UIShape::UIShape(Scene* scene, PathPoint start, PathPoint end, const Attribute& attribute) : mAtt(attribute)
{
	mAtt.shapeType = ShapeType::Path;
	mAtt.shapeAtt = ShapeAttribute::Stroke;
	mAtt.cursorType = CursorType::None;
	rScene = scene;
	if (start.type == PathPoint::Command::MoveTo)
	{
		mEntity = rScene->createPathLayer({start, end});
	}
	else
	{
		auto intermediate = start;
		start.type = PathPoint::Command::MoveTo;
		mEntity = rScene->createPathLayer({start, intermediate, end});
	}

	init();
	mEntity.update();
}

UIShape::UIShape(Scene* scene, Vec2 center, float w, const Attribute& att) : UIShape(scene, center, Vec2(w, w), att)
{
}

UIShape::UIShape(Scene* scene, const std::array<Vec2, 4>& obbPoints, const Attribute& att) : mAtt(att)
{
	mAtt.shapeType = ShapeType::Path;
	mAtt.shapeAtt = ShapeAttribute::Stroke;

	rScene = scene;
	mObbPoints = obbPoints;
	mEntity = rScene->createObb(obbPoints);

	init();
	mEntity.update();
}

UIShape::~UIShape()
{
	rScene->destroyEntity(mEntity);
}

void UIShape::moveTo(const Vec2& xy)
{
	auto& transform = mEntity.getComponent<TransformComponent>();
	transform.localPosition = xy;
	mEntity.update();

	auto& shape = mEntity.getComponent<ShapeComponent>();
	mObbPoints = GetObb(shape.shape);
}
void UIShape::moveByDelta(const Vec2& delta)
{
	mEntity.moveByDelta(delta);
	mEntity.update();

	auto& shape = mEntity.getComponent<ShapeComponent>();
	mObbPoints = GetObb(shape.shape);
}
bool UIShape::onStartLeftDown(Vec2 xy)
{
	if (mAtt.cursorType == CursorType::None || !isVisible())
		return false;

	if (IsInner(mObbPoints, xy))
	{
		mState = State::Pressed;
		return true;
	}
	return false;
}

bool UIShape::onDragLeftMouse()
{
	if (mOnLeftDrag)
	{
		return mOnLeftDrag->invoke();
	}
	return false;
}

bool UIShape::onEndLeftMouse()
{
	mState = State::None;
	return false;
}

bool UIShape::onMoveMouse(const Vec2& xy)
{
	if (mAtt.cursorType == CursorType::None || !isVisible())
		return false;

	auto beforeState = mState;
	if (mState != State::Pressed)
	{
		if (IsInner(mObbPoints, xy))
		{
			mState = State::Hovered;
		}
		else
		{
			mState = State::Normal;
		}
		if (mState == beforeState)
		{
			return false;
		}
	}

	StateAttribute* currentStateColor{nullptr};

	switch (mState)
	{
		case State::Normal:
		{
			currentStateColor = &mAtt.normal;
			break;
		}
		case State::Pressed:
		{
			currentStateColor = &mAtt.pressed;
			break;
		}
		case State::Hovered:
		{
			currentStateColor = &mAtt.hovered;
			break;
		}
	}

	if (mEntity.hasComponent<SolidFillComponent>())
	{
		auto& fill = mEntity.getComponent<SolidFillComponent>();
		fill.color = currentStateColor->fillColor;
		fill.alpha = currentStateColor->fillAlpha;
	}

	if (mEntity.hasComponent<StrokeComponent>())
	{
		auto& fill = mEntity.getComponent<StrokeComponent>();
		fill.color = currentStateColor->strokeColor;
		fill.alpha = currentStateColor->strokeAlpha;
	}
	mEntity.updateShapeAtt();

	return false;
}

void UIShape::updatePath(PathPoints pathPoints)
{
	auto* path = mEntity.findPath<RawPath>();
	if (path && pathPoints.size() > 0)
	{
		if (pathPoints[0].type != PathPoint::Command::MoveTo)
		{
			pathPoints.insert(pathPoints.begin(), pathPoints[0]);
			pathPoints.begin()->type = PathPoint::Command::MoveTo;
		}
		path->path = pathPoints;
		init();
		Update(mEntity.getComponent<ShapeComponent>(), *path);
	}
}

Vec2 UIShape::getWorldPosition()
{
	return mEntity.getComponent<WorldTransformComponent>().worldPosition;
}

void UIShape::setVisible(bool visible)
{
	visible ? mEntity.show() : mEntity.hide();
}

bool UIShape::isVisible() const
{
	return !mEntity.isHidden();
}

void UIShape::init()
{
	// update order: normal -> hovered,pressed
	std::array<StateAttribute*, 3> attributes = {&mAtt.normal, &mAtt.hovered, &mAtt.pressed};

	if (mEntity.hasComponent<SolidFillComponent>())
	{
		auto& fill = mEntity.getComponent<SolidFillComponent>();

		for (auto* attribute : attributes)
		{
			if (attribute->fillColor.r == -1.0f)
			{
				if (&mAtt.normal != attribute)
					attribute->fillColor = mAtt.normal.fillColor;
				else
					attribute->fillColor = CommonSetting::Color_DefaultControlBoxInner;
			}
			if (attribute->fillAlpha == -1.0f)
			{
				if (&mAtt.normal != attribute)
					attribute->fillAlpha = mAtt.normal.fillAlpha;
				else
					attribute->fillAlpha = 255.0f;
			}
		}
		fill.color = mAtt.normal.fillColor;
		fill.alpha = mAtt.normal.fillAlpha;
	}

	if (mEntity.hasComponent<StrokeComponent>())
	{
		auto& stroke = mEntity.getComponent<StrokeComponent>();

		for (auto* attribute : attributes)
		{
			if (attribute->strokeColor.r == -1.0f)
			{
				if (&mAtt.normal != attribute)
					attribute->strokeColor = mAtt.normal.strokeColor;
				else
					attribute->strokeColor = CommonSetting::Color_DefaultControlBoxOutline;
			}
			if (attribute->strokeAlpha == -1.0f)
			{
				if (&mAtt.normal != attribute)
					attribute->strokeAlpha = mAtt.normal.strokeAlpha;
				else
					attribute->strokeAlpha = 255.0f;
			}
			if (attribute->strokeWidth == -1.0f)
			{
				if (&mAtt.normal != attribute)
					attribute->strokeWidth = mAtt.normal.strokeWidth;
				else
					attribute->strokeWidth = CommonSetting::Width_DefaultStroke;
			}
		}
		stroke.color = mAtt.normal.strokeColor;
		stroke.alpha = mAtt.normal.strokeAlpha;
		stroke.width = mAtt.normal.strokeWidth;
	}
	switch (mAtt.shapeType)
	{
		case ShapeType::Rect:
		case ShapeType::Ellipse:
		case ShapeType::Polygon:
		{
			auto& shape = mEntity.getComponent<ShapeComponent>();
			mObbPoints = GetObb(shape.shape);
		}
	}
}

}	 // namespace core