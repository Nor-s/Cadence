#ifndef _CORE_SCENE_SCENE_H_
#define _CORE_SCENE_SCENE_H_

#include "entity.h"

#include "common/common.h"

#include <thorvg.h>
#include <unordered_map>
#include <array>
#include <list>

using reactive_storage = entt::reactive_mixin<entt::basic_storage<void>>;

namespace core
{

class CanvasWrapper;
class Entity;

class Scene
{
public:
	static Entity CreateEntity(Scene* scene, std::string_view name, Entity parent);
	static Scene* FindScene(tvg::Scene* scene);
	static Entity FindEntity(uint32_t entityId);

protected:
	static std::unordered_map<tvg::Scene*, Scene*> gSceneMap;
	static std::unordered_map<uint32_t, Entity> gEntityMap;

public:
	Scene(Scene* parentScene = nullptr);
	~Scene();
	entt::registry& getRegistry()
	{
		return mRegistry;
	}
	std::unique_ptr<Scene> createScene();
	Entity createEntity(std::string_view name);
	Entity createEllipseFillLayer(Vec2 minXy, Vec2 wh);
	Entity createEllipseFillStrokeLayer(Vec2 minXy, Vec2 wh);
	Entity createRectFillLayer(Vec2 minXy, Vec2 wh);
	Entity createRectFillStrokeLayer(Vec2 minXy, Vec2 wh);
	Entity createPolygonFillLayer(Vec2 minXy, Vec2 wh);
	Entity createPolygonFillStrokeLayer(Vec2 minXy, Vec2 wh);
	Entity createStarFillLayer(Vec2 minXy, Vec2 wh);

	// a deep-copied PathLayer using the first point of the pathList as the origin.
	Entity createPathLayer(PathPoints pathList);
	Entity createObb(const std::array<Vec2, 4>& points);

	template <typename T>
	std::vector<Entity> findByComponent()
	{
		std::vector<Entity> ret;

		mRegistry.view<T>().each([&](entt::entity entity, T& comp)
								 { ret.emplace_back(this, static_cast<uint32_t>(entity)); });

		return ret;
	}

	Entity getEntityById(uint32_t id);
	Entity tryGetEntityById(uint32_t id);

	void destroyEntity(Entity& entity);
	void pushCanvas(CanvasWrapper* canvas);
	void reorder();

	void changeDrawOrder(const Entity& entity, ChangeOrderType changeOrderType);

	tvg::Scene* getScene()
	{
		return mTvgScene;
	}

	bool onUpdate();
	void destroy();

	const std::list<Entity>& getDrawOrder();

	uint32_t mId;
	Entity mSceneEntity;

protected:
	friend class Entity;
	friend class AnimationCreatorCanvas;
	entt::registry mRegistry{};
	std::vector<CanvasWrapper*> rCanvasList;
	std::list<Entity> mDrawOrder;
	tvg::Scene* mTvgScene;
	reactive_storage mStorage;
	bool mIsDirty{false};
	Scene* rParentScene{nullptr};
};

}	 // namespace core

#include "entity.hpp"

#endif
