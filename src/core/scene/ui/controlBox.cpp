#include "controlBox.h"

#include "common/common.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"

#include "canvas/shapeUtil.h"

namespace core
{

ControlOverlay::ControlOverlay(Scene* scene, Vec2 center, Vec2 wh, Type type, ShapeType shapeType)
{
	mType = type;
	rScene = scene;
	switch (shapeType)
	{
		case ShapeType::ActiveFillStrokeEllipse:
		case ShapeType::FillStrokeEllipse:
		case ShapeType::TransparentEllipse:
		case ShapeType::StrokeEllipse:
		{
			mEntity = rScene->createEllipseFillStrokeLayer(center - wh / 2.0f, wh);
			break;
		}
		case ShapeType::FillStrokeRect:
		case ShapeType::StrokeRect:
		{
			mEntity = rScene->createRectFillStrokeLayer(center - wh / 2.0f, wh);
			break;
		}
	}
	auto& stroke = mEntity.getComponent<StrokeComponent>();
	auto& fill = mEntity.getComponent<SolidFillComponent>();
	stroke.color = CommonSetting::Color_DefaultControlBoxOutline;
	fill.color = CommonSetting::Color_DefaultControlBoxInner;

	switch (shapeType)
	{
		case ShapeType::StrokeEllipse:
		case ShapeType::StrokeRect:
		case ShapeType::TransparentEllipse:
		{
			fill.alpha = 0.0f;
			break;
		}
		case ShapeType::ActiveFillStrokeEllipse:
		{
			fill.color = CommonSetting::Color_DefaultActiveFillStrokeEllipse;
			break;
		}
	}
	switch (shapeType)
	{
		case ShapeType::TransparentEllipse:
		{
			stroke.alpha = 0.0f;
			break;
		}
	}

	auto& shape = mEntity.getComponent<ShapeComponent>();
	mObbPoints = GetObb(shape.shape);
}

ControlOverlay::ControlOverlay(Scene* scene, PathPoint start, PathPoint end)
{
	mType = Type::None;
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

	auto& stroke = mEntity.getComponent<StrokeComponent>();
	stroke.color = CommonSetting::Color_DefaultControlBoxOutline;
}

ControlOverlay::ControlOverlay(Scene* scene, Vec2 center, float w, Type type, ShapeType shape)
	: ControlOverlay(scene, center, Vec2(w, w), type, shape)
{
}

ControlOverlay::ControlOverlay(Scene* scene, const std::array<Vec2, 4>& obbPoints, Type type)
{
	mType = type;
	rScene = scene;
	mObbPoints = obbPoints;
	mEntity = rScene->createObb(obbPoints);

	auto& stroke = mEntity.getComponent<StrokeComponent>();
	stroke.color = CommonSetting::Color_DefaultControlBoxOutline;
}

ControlOverlay::~ControlOverlay()
{
	rScene->destroyEntity(mEntity);
}

void ControlOverlay::moveTo(const Vec2& xy)
{
	auto& transform = mEntity.getComponent<TransformComponent>();
	transform.localCenterPosition = xy;
	mEntity.update();

	auto& shape = mEntity.getComponent<ShapeComponent>();
	mObbPoints = GetObb(shape.shape);
}
void ControlOverlay::moveByDelta(const Vec2& delta)
{
	mEntity.moveByDelta(delta);
	mEntity.update();

	auto& shape = mEntity.getComponent<ShapeComponent>();
	mObbPoints = GetObb(shape.shape);
}
bool ControlOverlay::onLeftDown(Vec2 xy)
{
	if (mType == Type::None)
		return false;

	if (IsInner(mObbPoints, xy))
	{
		return true;
	}
	return false;
}

void ControlOverlay::setVisible(bool visible)
{
	visible ? mEntity.show() : mEntity.hide();
}

bool ControlOverlay::isVisible() const
{
	return !mEntity.isHidden();
}

}	 // namespace core