#include "entity.h"
#include "scene.h"

#include "component/components.h"

namespace core
{

Entity::Entity(Scene* scene, uint32_t id)
{
	rScene = scene;
	mHandle = (entt::entity) id;
}
Entity::Entity(Scene* scene)
{
	rScene = scene;
	mHandle = scene->mRegistry.create();
}
void Entity::moveByDelta(const Vec2& delta)
{
	auto& transform = getComponent<TransformComponent>();
	transform.localPosition = transform.localPosition + delta;
}
void Entity::hide()
{
	if (mHandle == entt::null || mIsHide == true || hasComponent<ShapeComponent>() == false)
		return;

	if (auto& shape = getComponent<ShapeComponent>(); shape.shape != nullptr)
	{
		shape.shape->visible(false);
		mIsHide = true;
	}
}
void Entity::show()
{
	if (mHandle == entt::null || mIsHide == false || hasComponent<ShapeComponent>() == false)
		return;

	if (auto& shape = getComponent<ShapeComponent>(); shape.shape != nullptr)
	{
		shape.shape->visible(true);
		mIsHide = false;
	}
}
void Entity::setDirty(Dirty::Type dirtyType)
{
	auto& dirty = getComponent<Dirty>();

	rScene->mRegistry.patch<Dirty>(mHandle);

	dirty.mask = dirty | dirtyType;
	rScene->mIsDirty = true;
}
void Entity::update()
{
	updateTransform();
	updateShapePath();
	updateShapeAtt();
}

void Entity::updateShapePath()
{
	if (hasComponent<ShapeComponent>() == false)
		return;

	auto& shape = getComponent<ShapeComponent>();

	Reset(shape);

	if (hasComponent<PathListComponent>())
	{
		auto& pathList = getComponent<PathListComponent>();
		for (auto& path : pathList.paths)
		{
			path->appendTo(shape);
		}
	}
}

bool Entity::updateShapePath(float keyframeNo)
{
	if (hasComponent<ShapeComponent>() == false)
		return false;

	auto& shape = getComponent<ShapeComponent>();

	bool isChanged = false;
	if (hasComponent<PathListComponent>())
	{
		auto& pathList = getComponent<PathListComponent>();
		for (auto& path : pathList.paths)
		{
			isChanged |= path->update(keyframeNo);
			path->appendTo(shape);
		}
	}
	return isChanged;
}

void Entity::updateShapeAtt()
{
	if (hasComponent<ShapeComponent>() == false)
		return;

	auto& shape = getComponent<ShapeComponent>();

	UpdateShape<SolidFillComponent>(*this, shape);
	UpdateShape<StrokeComponent>(*this, shape);
}

void Entity::updateTransform()
{
	if (hasComponent<ShapeComponent>() == false)
	{
		auto& transform = getComponent<TransformComponent>();
		transform.update();
	}
	else
	{
		auto& shape = getComponent<ShapeComponent>();
		UpdateShape<TransformComponent>(*this, shape);
	}
}

const EntityID Entity::getId()
{
	return getComponent<IDComponent>().id;
}

const bool Entity::isNull() const
{
	return mHandle == entt::null;
}

void Entity::changeDrawOrder(ChangeOrderType changeOrderType)
{
	rScene->changeDrawOrder(*this, changeOrderType);
}

}	 // namespace core
