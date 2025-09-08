#include "scene.h"
#include "entity.h"
#include "component/components.h"
#include "component/uiComponents.h"

#include "canvas/animationCreatorCanvas.h"
#include "interface/editInterface.h"
#include "animation/animator.h"

#include "ui/bbox.h"
#include "canvas/shapeUtil.h"

#include <thorvg.h>

#include <algorithm>

namespace core
{
std::unordered_map<tvg::Scene*, Scene*> Scene::gSceneMap;
std::unordered_map<uint32_t, Entity> Scene::gEntityMap;

Scene::Scene(bool isMainScene) : mIsMainScene(isMainScene)
{
	auto entity = Scene::CreateEntity(this, "Scene");
	auto& scene = entity.addComponent<SceneComponent>();
	mId = entity.getComponent<IDComponent>().id;

	mTvgScene = tvg::Scene::gen();
	mTvgScene->ref();
	mTvgScene->id = mId;
	scene.scene = this;
	gSceneMap[mTvgScene] = this;
}

Scene::~Scene()
{
	mRegistry.view<ShapeComponent>().each([](auto entity, auto& shape) { shape.shape->unref(); });
	mTvgScene->unref();
}

Entity Scene::CreateEntity(Scene* scene, std::string_view name)
{
	static uint32_t id = 1;

	Entity entity(scene);

	entity.addComponent<IDComponent>(id);
	entity.addComponent<TransformComponent>();
	entity.addComponent<NameComponent>(name.empty() ? "Entity" : name);
	entity.addComponent<RelationshipComponent>();
	scene->gEntityMap[id] = entity;

	id++;

	scene->mDrawOrder.push_back(entity);

	return entity;
}

Scene* Scene::FindScene(tvg::Scene* scene)
{
	return gSceneMap[scene];
}

Entity Scene::FindEntity(uint32_t entityId)
{
	if (auto it = gEntityMap.find(entityId); it != gEntityMap.end())
	{
		return it->second;
	}
	return Entity();
}

Entity Scene::createEntity(std::string_view name)
{
	return CreateEntity(this, name);
}

Entity Scene::createEllipseFillLayer(Vec2 minXy, Vec2 wh)
{
	auto entity = CreateEntity(this, "Ellipse Layer");
	auto& id = entity.getComponent<IDComponent>();
	auto& transform = entity.getComponent<TransformComponent>();
	auto& rect = entity.addComponent<ElipsePathComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	entity.addComponent<SolidFillComponent>();

	transform.localPosition = minXy + wh * 0.5f;
	transform.anchorPoint = {0.0f, 0.0f};
	rect.scale = wh;
	rect.position = {0.0f, 0.0f};

	// push shape
	shape.shape = tvg::Shape::gen();
	shape.shape->id = id.id;
	shape.shape->ref();

	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}

Entity Scene::createEllipseFillStrokeLayer(Vec2 minXy, Vec2 wh)
{
	Entity entity = createEllipseFillLayer(minXy, wh);
	entity.addComponent<StrokeComponent>();
	entity.update();
	return entity;
}

Entity Scene::createRectFillLayer(Vec2 minXy, Vec2 wh)
{
	auto entity = CreateEntity(this, "Rect Layer");
	auto& id = entity.getComponent<IDComponent>();
	auto& transform = entity.getComponent<TransformComponent>();
	auto& rect = entity.addComponent<RectPathComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	entity.addComponent<SolidFillComponent>();

	transform.localPosition = minXy + wh * 0.5f;
	transform.anchorPoint = {0.0f, 0.0f};
	rect.scale = wh;
	rect.radius = 0.0f;
	rect.position = {0.0f, 0.0f};

	// push shape
	shape.shape = tvg::Shape::gen();
	shape.shape->id = id.id;
	shape.shape->ref();

	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}
Entity Scene::createRectFillStrokeLayer(Vec2 minXy, Vec2 wh)
{
	Entity entity = createRectFillLayer(minXy, wh);
	entity.addComponent<StrokeComponent>();
	entity.update();
	return entity;
}

Entity Scene::createPolygonFillLayer(Vec2 minXy, Vec2 wh)
{
	auto entity = CreateEntity(this, "polygon");
	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& polygon = entity.addComponent<PolygonPathComponent>();

	entity.addComponent<SolidFillComponent>();

	const auto radius = wh.h / 2.0f;
	polygon.points = CommonSetting::Count_DefaultPolygonPathPoint;
	polygon.outerRadius = radius;

	transform.anchorPoint = {0.0f, 0.0f};
	transform.localPosition = polygon.path.center = minXy + wh * 0.5f;
	transform.scale.x = wh.w * 0.5f / radius;

	shape.shape = tvg::Shape::gen();
	shape.shape->ref();
	shape.shape->id = id.id;

	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}

Entity Scene::createPolygonFillStrokeLayer(Vec2 minXy, Vec2 wh)
{
	auto entity = createPolygonFillLayer(minXy, wh);
	entity.addComponent<StrokeComponent>();
	return entity;
}

Entity Scene::createStarFillLayer(Vec2 minXy, Vec2 wh)
{
	if (wh.w * wh.h < 1e-6)
	{
		wh.w = wh.h = 1.0f;
	}
	auto entity = CreateEntity(this, "Star");
	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& star = entity.addComponent<StarPolygonPathComponent>();

	entity.addComponent<SolidFillComponent>();

	const auto radius = wh.h / 2.0f;
	star.points = CommonSetting::Count_DefaultStarPolygonPathPoint;
	star.outerRadius = radius;
	star.innerRadius = radius / 2.0f;

	transform.anchorPoint = {0.0f, 0.0f};
	star.path.center = transform.localPosition = minXy + wh * 0.5f;
	transform.scale.x = wh.w * 0.5f / radius;

	shape.shape = tvg::Shape::gen();
	shape.shape->ref();
	shape.shape->id = id.id;

	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}

Entity Scene::createPathLayer(PathPoints path)
{
	assert(path.empty() == false);

	auto entity = CreateEntity(this, "path");
	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& pathComponent = entity.addComponent<PathComponent>();
	auto& stroke = entity.addComponent<StrokeComponent>();

	stroke.width = CommonSetting::Width_DefaultPathLine;

	transform.anchorPoint = {0.0f, 0.0f};
	transform.localPosition = path[0].localPosition;

	for (auto& point : path)
	{
		point.localPosition = point.localPosition - transform.localPosition;
	}

	shape.shape = tvg::Shape::gen();
	shape.shape->ref();
	shape.shape->id = id.id;

	pathComponent.path = path;
	pathComponent.center = transform.localPosition;

	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}

Entity Scene::createObb(const std::array<Vec2, 4>& points)
{
	auto entity = CreateEntity(this, "obb");

	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& path = entity.addComponent<PathComponent>();
	auto& stroke = entity.addComponent<StrokeComponent>();

	auto minx = std::min({points[0].x, points[1].x, points[2].x, points[3].x});
	auto maxx = std::max({points[0].x, points[1].x, points[2].x, points[3].x});
	auto miny = std::min({points[0].y, points[1].y, points[2].y, points[3].y});
	auto maxy = std::max({points[0].y, points[1].y, points[2].y, points[3].y});

	auto width = maxx - minx;
	auto height = maxy - miny;

	transform.anchorPoint = {0.0f, 0.0f};	 // oring of the local, center of the bbox
	transform.localPosition = {minx + width * 0.5f, miny + height * 0.5f};

	auto centerp = transform.localPosition;
	path.path.resize(5);
	path.path[0].type = PathPoint::Command::MoveTo;
	path.path[1].type = PathPoint::Command::LineTo;
	path.path[2].type = PathPoint::Command::LineTo;
	path.path[3].type = PathPoint::Command::LineTo;
	path.path[4].type = PathPoint::Command::Close;
	path.center = Vec2{width / 2, height / 2};

	for (int i = 0; i < 4; i++)
	{
		path.path[i].localPosition = points[i] - centerp;
	}

	shape.shape = tvg::Shape::gen();
	shape.shape->ref();
	shape.shape->id = id.id;

	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}

Entity Scene::getEntityById(uint32_t id)
{
	auto it = gEntityMap.find(id);
	assert(it != gEntityMap.end());
	return it->second;
}
Entity Scene::tryGetEntityById(uint32_t id)
{
	auto it = gEntityMap.find(id);
	if (it == gEntityMap.end())
		return Entity();
	return it->second;
}

void Scene::destroyEntity(Entity& entity)
{
	mDrawOrder.erase(std::remove_if(mDrawOrder.begin(), mDrawOrder.end(),
									[&entity](Entity& a) { return entity.getId() == a.getId(); }));

	// LOG_INFO("Destroying entity: {}", entity.getComponent<IDComponent>().id);
	if (entity.hasComponent<ShapeComponent>())
	{
		auto& shape = entity.getComponent<ShapeComponent>();
		mTvgScene->remove(shape.shape);
		shape.shape->unref();
	}
	gEntityMap.erase(entity.getComponent<IDComponent>().id);
	mRegistry.destroy(entity.mHandle);
	entity.mHandle = entt::null;
}
void Scene::pushCanvas(CanvasWrapper* canvas)
{
	rCanvasList.push_back(canvas);
}
void Scene::reorder()
{
	for (auto& entity : mDrawOrder)
	{
		if (entity.hasComponent<SceneComponent>())
		{
			auto& scene = entity.getComponent<SceneComponent>();
			if (this != scene.scene)
			{
				mTvgScene->remove(scene.scene->mTvgScene);
			}
		}
		else if (entity.hasComponent<ShapeComponent>())
		{
			auto& shape = entity.getComponent<ShapeComponent>();
			mTvgScene->remove(shape.shape);
		}
	}

	for (auto& entity : mDrawOrder)
	{
		if (entity.hasComponent<SceneComponent>())
		{
			auto& scene = entity.getComponent<SceneComponent>();

			if (this != scene.scene)
			{
				scene.scene->reorder();
				mTvgScene->push(scene.scene->mTvgScene);
			}
		}
		else if (entity.hasComponent<ShapeComponent>())
		{
			auto& shape = entity.getComponent<ShapeComponent>();
			mTvgScene->push(shape.shape);
		}
	}
}

void Scene::changeDrawOrder(const Entity& entity, ChangeOrderType changeOrderType)
{
	auto it = std::find(mDrawOrder.begin(), mDrawOrder.end(), entity);
	if (it == mDrawOrder.end())
	{
		return;
	}
	int idx = it - mDrawOrder.begin();
	int nextIdx = 0;
	switch (changeOrderType)
	{
		case ChangeOrderType::ToFront:
		{
			nextIdx = mDrawOrder.size() - 1;
			break;
		}
		case ChangeOrderType::ToBack:
		{
			nextIdx = 0;
			break;
		}
		case ChangeOrderType::ToForward:
		{
			nextIdx = idx + 1;
			break;
		}
		case ChangeOrderType::ToBackward:
		{
			nextIdx = idx - 1;
			break;
		}
	};
	if (nextIdx == idx || nextIdx < 0 || mDrawOrder.size() <= nextIdx)
	{
		return;
	}

	std::swap(mDrawOrder[idx], mDrawOrder[nextIdx]);

	reorder();
}

void Scene::onUpdate()
{
	// todo: this canvas maybe no scene owner (current canvas count == 1)
	auto* canvasPtr = GetCurrentAnimCanvas();

	mRegistry.view<BBoxControlComponent>().each([](auto entity, BBoxControlComponent& bbox) { bbox.bbox->onUpdate(); });

	auto* animCanvas = static_cast<AnimationCreatorCanvas*>(canvasPtr);
	auto* animator = animCanvas->mAnimator.get();
	const auto keyframeNo = animator->mCurrentFrameNo;
	{
		mRegistry.view<TransformComponent, TransformKeyframeComponent>().each(
			[keyframeNo](auto entity, TransformComponent& transform, TransformKeyframeComponent& keyframes)
			{
				if (keyframes.positionKeyframes.isEnable)
					transform.localPosition = keyframes.positionKeyframes.frame(keyframeNo);
				if (keyframes.scaleKeyframes.isEnable)
					transform.scale = keyframes.scaleKeyframes.frame(keyframeNo);
				if (keyframes.rotationKeyframes.isEnable)
					transform.rotation = keyframes.rotationKeyframes.frame(keyframeNo);
			});
	}

	mRegistry.view<TransformComponent, PathComponent, ShapeComponent>().each(
		[](auto entity, TransformComponent& transform, PathComponent& path, ShapeComponent& shape)
		{
			Update(shape, transform);
			Update(shape, path);
		});
	mRegistry.view<TransformComponent, ElipsePathComponent, ShapeComponent>().each(
		[keyframeNo](auto entity, TransformComponent& transform, ElipsePathComponent& path, ShapeComponent& shape)
		{
			path.update(keyframeNo);
			Update(shape, transform);
			Update(shape, path);
		});
	mRegistry.view<TransformComponent, RectPathComponent, ShapeComponent>().each(
		[keyframeNo](auto entity, TransformComponent& transform, RectPathComponent& path, ShapeComponent& shape)
		{
			path.update(keyframeNo);
			Update(shape, transform);
			Update(shape, path);
		});
	mRegistry.view<TransformComponent, StarPolygonPathComponent, ShapeComponent>().each(
		[keyframeNo](auto entity, TransformComponent& transform, StarPolygonPathComponent& path, ShapeComponent& shape)
		{
			path.update(keyframeNo);
			Update(shape, transform);
			Update(shape, path);
		});
	mRegistry.view<TransformComponent, PolygonPathComponent, ShapeComponent>().each(
		[keyframeNo](auto entity, TransformComponent& transform, PolygonPathComponent& path, ShapeComponent& shape)
		{
			path.update(keyframeNo);
			Update(shape, transform);
			Update(shape, path);
		});

	mRegistry.view<ShapeComponent, SolidFillComponent>().each(
		[keyframeNo](auto entity, ShapeComponent& shape, SolidFillComponent& fill)
		{
			fill.update(keyframeNo);
			Update(shape, fill);
		});
	mRegistry.view<ShapeComponent, StrokeComponent>().each(
		[keyframeNo](auto entity, ShapeComponent& shape, StrokeComponent& stroke)
		{
			stroke.update(keyframeNo);
			Update(shape, stroke);
		});

	mRegistry.view<SceneComponent>().each(
		[this](auto entity, SceneComponent& scene)
		{
			if (scene.scene->mId != this->mId)
			{
				scene.scene->onUpdate();
			}
		});
}

}	 // namespace core