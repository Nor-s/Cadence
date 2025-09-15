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

#include <entt/entt.hpp>

namespace core
{
std::unordered_map<tvg::Scene*, Scene*> Scene::gSceneMap;
std::unordered_map<uint32_t, Entity> Scene::gEntityMap;

Scene::Scene(Scene* parentScene)
{
	Scene* targetScene = parentScene ? parentScene : this;
	rParentScene = parentScene;

	auto entity = Scene::CreateEntity(targetScene, "Scene", parentScene ? parentScene->mSceneEntity : Entity());
	auto& scene = entity.addComponent<SceneComponent>();
	mId = entity.getComponent<IDComponent>().id;

	mTvgScene = tvg::Scene::gen();
	mTvgScene->ref();
	mTvgScene->id = mId;
	scene.scene = this;
	gSceneMap[mTvgScene] = this;

	mStorage.bind(mRegistry);
	mStorage.on_update<Dirty>();

	if (parentScene)
	{
		parentScene->mTvgScene->push(mTvgScene);
	}
	mSceneEntity = entity;
}

Scene::~Scene()
{
	// todo: parentScene -> remove this scene?

	mStorage.clear();
	mRegistry.on_update<Dirty>().disconnect(&mStorage);
	mRegistry.view<ShapeComponent>().each([](auto entity, auto& shape) { shape.shape->unref(); });
	mTvgScene->unref();
}

Entity Scene::CreateEntity(Scene* scene, std::string_view name, Entity parent)
{
	static uint32_t id = 1;

	Entity entity(scene);

	entity.addComponent<IDComponent>(id);
	auto& transform = entity.addComponent<TransformComponent>();
	entity.addComponent<WorldTransformComponent>(
		transform, parent.isNull() ? nullptr : &parent.getComponent<WorldTransformComponent>());
	entity.addComponent<NameComponent>(name.empty() ? "Entity" : name);
	entity.addComponent<RelationshipComponent>();
	entity.addComponent<Dirty>();
	scene->gEntityMap[id] = entity;

	id++;

	scene->mDrawOrder.push_back(entity);

	scene->mIsDirty = true;
	entity.setDirty();

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

std::unique_ptr<Scene> Scene::createScene()
{
	return std::unique_ptr<Scene>();
}

Entity Scene::createEntity(std::string_view name)
{
	return CreateEntity(this, name, mSceneEntity);
}

Entity Scene::createEllipseFillLayer(Vec2 minXy, Vec2 wh)
{
	auto entity = CreateEntity(this, "Ellipse Layer", mSceneEntity);
	auto& id = entity.getComponent<IDComponent>();
	auto& transform = entity.getComponent<TransformComponent>();
	auto& pathList = entity.addComponent<PathListComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	entity.addComponent<SolidFillComponent>();
	auto ellipse = std::make_unique<EllipsePath>();

	transform.localPosition = minXy + wh * 0.5f;
	transform.anchorPoint = {0.0f, 0.0f};
	ellipse->scale = wh;
	ellipse->position = {0.0f, 0.0f};

	// push shape
	shape.shape = tvg::Shape::gen();
	shape.shape->id = id.id;
	shape.shape->ref();

	pathList.paths.push_back(std::move(ellipse));
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
	auto entity = CreateEntity(this, "Rect Layer", mSceneEntity);
	auto& id = entity.getComponent<IDComponent>();
	auto& transform = entity.getComponent<TransformComponent>();
	auto& pathList = entity.addComponent<PathListComponent>();
	auto rect = std::make_unique<RectPath>();
	auto& shape = entity.addComponent<ShapeComponent>();
	entity.addComponent<SolidFillComponent>();

	transform.localPosition = minXy + wh * 0.5f;
	transform.anchorPoint = {0.0f, 0.0f};
	rect->scale = wh;
	rect->radius = 0.0f;
	rect->position = {0.0f, 0.0f};

	// push shape
	shape.shape = tvg::Shape::gen();
	shape.shape->id = id.id;
	shape.shape->ref();

	pathList.paths.push_back(std::move(rect));
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
	auto entity = CreateEntity(this, "Polygon Layer", mSceneEntity);
	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& pathList = entity.addComponent<PathListComponent>();
	auto polygon = std::make_unique<PolygonPath>();

	entity.addComponent<SolidFillComponent>();

	const auto radius = wh.h / 2.0f;
	polygon->points = CommonSetting::Count_DefaultPolygonPathPoint;
	polygon->outerRadius = radius;

	transform.anchorPoint = {0.0f, 0.0f};
	transform.localPosition = polygon->path.center = minXy + wh * 0.5f;
	transform.scale.x = wh.w * 0.5f / radius;

	shape.shape = tvg::Shape::gen();
	shape.shape->ref();
	shape.shape->id = id.id;

	pathList.paths.push_back(std::move(polygon));
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
	auto entity = CreateEntity(this, "Star Layer", mSceneEntity);
	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& pathList = entity.addComponent<PathListComponent>();
	auto star = std::make_unique<StarPolygonPath>();

	entity.addComponent<SolidFillComponent>();

	const auto radius = wh.h / 2.0f;
	star->points = CommonSetting::Count_DefaultStarPolygonPathPoint;
	star->outerRadius = radius;
	star->innerRadius = radius / 2.0f;

	transform.anchorPoint = {0.0f, 0.0f};
	star->path.center = transform.localPosition = minXy + wh * 0.5f;
	transform.scale.x = wh.w * 0.5f / radius;

	shape.shape = tvg::Shape::gen();
	shape.shape->ref();
	shape.shape->id = id.id;

	pathList.paths.push_back(std::move(star));
	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}

Entity Scene::createPathLayer(PathPoints path)
{
	assert(path.empty() == false);

	auto entity = CreateEntity(this, "Path Layer", mSceneEntity);
	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& pathList = entity.addComponent<PathListComponent>();
	auto& stroke = entity.addComponent<StrokeComponent>();
	auto rawPath = std::make_unique<RawPath>();

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

	rawPath->path = path;
	rawPath->center = transform.localPosition;

	pathList.paths.push_back(std::move(rawPath));
	entity.update();
	mTvgScene->push(shape.shape);

	return entity;
}

Entity Scene::createObb(const std::array<Vec2, 4>& points)
{
	auto entity = CreateEntity(this, "obb", mSceneEntity);

	auto& transform = entity.getComponent<TransformComponent>();
	auto& id = entity.getComponent<IDComponent>();
	auto& shape = entity.addComponent<ShapeComponent>();
	auto& stroke = entity.addComponent<StrokeComponent>();
	auto& pathList = entity.addComponent<PathListComponent>();
	auto rawPath = std::make_unique<RawPath>();

	auto minx = std::min({points[0].x, points[1].x, points[2].x, points[3].x});
	auto maxx = std::max({points[0].x, points[1].x, points[2].x, points[3].x});
	auto miny = std::min({points[0].y, points[1].y, points[2].y, points[3].y});
	auto maxy = std::max({points[0].y, points[1].y, points[2].y, points[3].y});

	auto width = maxx - minx;
	auto height = maxy - miny;

	transform.anchorPoint = {0.0f, 0.0f};	 // oring of the local, center of the bbox
	transform.localPosition = {minx + width * 0.5f, miny + height * 0.5f};

	auto centerp = transform.localPosition;
	rawPath->path.resize(5);
	rawPath->path[0].type = PathPoint::Command::MoveTo;
	rawPath->path[1].type = PathPoint::Command::LineTo;
	rawPath->path[2].type = PathPoint::Command::LineTo;
	rawPath->path[3].type = PathPoint::Command::LineTo;
	rawPath->path[4].type = PathPoint::Command::Close;
	rawPath->center = Vec2{width / 2, height / 2};

	for (int i = 0; i < 4; i++)
	{
		rawPath->path[i].localPosition = points[i] - centerp;
	}

	shape.shape = tvg::Shape::gen();
	shape.shape->ref();
	shape.shape->id = id.id;

	pathList.paths.push_back(std::move(rawPath));
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
		entity.removeComponent<ShapeComponent>();
	}
	gEntityMap.erase(entity.getComponent<IDComponent>().id);
	entity.getOrAddComponent<DestroyState>();

	entity.mHandle = entt::null;
	mIsDirty = true;
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
	mIsDirty = true;
}

void Scene::changeDrawOrder(const Entity& entity, ChangeOrderType changeOrderType)
{
	auto it = std::find(mDrawOrder.begin(), mDrawOrder.end(), entity);
	if (it == mDrawOrder.end() || mDrawOrder.size() < 2)
		return;

	switch (changeOrderType)
	{
		case ChangeOrderType::ToFront:
		{
			auto last = std::prev(mDrawOrder.end());
			if (it == last)
				return;
			mDrawOrder.splice(mDrawOrder.end(), mDrawOrder, it);
			break;
		}
		case ChangeOrderType::ToBack:
		{
			if (it == mDrawOrder.begin())
				return;
			mDrawOrder.splice(mDrawOrder.begin(), mDrawOrder, it);
			break;
		}
		case ChangeOrderType::ToForward:
		{
			auto next = std::next(it);
			if (next == mDrawOrder.end())
				return;
			mDrawOrder.splice(std::next(next), mDrawOrder, it);
			break;
		}
		case ChangeOrderType::ToBackward:
		{
			if (it == mDrawOrder.begin())
				return;
			auto prev = std::prev(it);
			mDrawOrder.splice(prev, mDrawOrder, it);
			break;
		}
	}

	reorder();
}
bool Scene::onUpdate()
{
	// todo: this canvas maybe no scene owner (current canvas count == 1)
	auto* canvasPtr = GetCurrentAnimCanvas();

	mRegistry.view<BBoxControlComponent>().each([](auto entity, BBoxControlComponent& bbox) { bbox.bbox->onUpdate(); });

	auto* animCanvas = static_cast<AnimationCreatorCanvas*>(canvasPtr);
	auto* animator = animCanvas->mAnimator.get();
	const auto keyframeNo = animator->mCurrentFrameNo;
	bool isStop = animator->mIsStop;

	mRegistry.view<TransformComponent, TransformKeyframeComponent>().each(
		[keyframeNo, scene = this](auto entity, TransformComponent& transform, TransformKeyframeComponent& keyframes)
		{
			if (keyframes.update(keyframeNo, transform))
			{
				Entity(scene, (uint32_t) entity).setDirty(core::Dirty::Type::Transform);
			}
		});

	// if (!isStop)
	{
		mRegistry.view<PathListComponent>().each(
			[keyframeNo, scene = this](auto entity, PathListComponent& path)
			{
				bool isChanged = false;
				for (auto& p : path.paths)
				{
					isChanged |= p->update(keyframeNo);
				}
				if (isChanged)
					Entity(scene, (uint32_t) entity).setDirty(core::Dirty::Type::Path);
			});
		mRegistry.view<SolidFillComponent>().each(
			[keyframeNo, scene = this](auto entity, SolidFillComponent& fill)
			{
				if (fill.update(keyframeNo))
					Entity(scene, (uint32_t) entity).setDirty(core::Dirty::Type::Fill);
			});

		mRegistry.view<StrokeComponent>().each(
			[keyframeNo, scene = this](auto entity, StrokeComponent& stroke)
			{
				if (stroke.update(keyframeNo))
					Entity(scene, (uint32_t) entity).setDirty(core::Dirty::Type::Stroke);
			});
	}

	for (auto& entity : mStorage)
	{
		auto& dirty = mRegistry.get<Dirty>(entity);
		auto e = Entity(this, (uint32_t) entity);

		if (e.hasComponent<DestroyState>())
		{
			continue;
		}

		if (e.hasComponent<ShapeComponent>())
		{
			auto& shape = e.getComponent<ShapeComponent>();
			if (HasDirty(dirty, Dirty::Type::Transform))
			{
				Update(shape, e.getComponent<TransformComponent>());
			}
			if (HasDirty(dirty, Dirty::Type::Path) || HasDirty(dirty, Dirty::Type::Fill) ||
				HasDirty(dirty, Dirty::Type::Stroke))
			{
				if (e.getComponent<PathListComponent>().paths.empty())
				{
					shape.shape->visible(false);
				}
				else
				{
					shape.shape->visible(true);
					e.updateShapePath();
				}
				if (e.hasComponent<SolidFillComponent>())
				{
					Update(shape, e.getComponent<SolidFillComponent>());
				}
				if (e.hasComponent<StrokeComponent>())
				{
					Update(shape, e.getComponent<StrokeComponent>());
				}
			}
		}
		if (e.hasComponent<SceneComponent>())
		{
			Update(e.getComponent<SceneComponent>(), e.getComponent<TransformComponent>());
		}

		dirty.mask = Dirty::Type::None;
	}

	mRegistry.view<WorldTransformComponent>().each([this](auto entity, WorldTransformComponent& world)
												   { world.update(); });

	mRegistry.view<SceneComponent>().each(
		[this](auto entity, SceneComponent& scene)
		{
			if (scene.scene != this)
				mIsDirty |= scene.scene->onUpdate();
		});

	bool isUpdate = false;
	if (mStorage.size() > 0 || mIsDirty)
	{
		isUpdate = true;
	}
	mStorage.clear();
	mIsDirty = false;

	return isUpdate;
}

void Scene::destroy()
{
	// todo: refactor [create - reference - update - destroy] ... life cycle
	mRegistry.view<DestroyState>().each([this](auto entity, DestroyState& destroy) { mRegistry.destroy(entity); });
}

const std::list<Entity>& Scene::getDrawOrder()
{
	return mDrawOrder;
}

}	 // namespace core