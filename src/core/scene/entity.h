#ifndef _CORE_SCENE_NSENTITY_H_
#define _CORE_SCENE_NSENTITY_H_

#include "../common/common.h"
#include <vector>
#include <memory>

#include <entt/entt.hpp>

namespace core
{
using EntityID = uint32_t;

struct ShapeComponent;
struct ElipsePathComponent;
struct RectPathComponent;
struct PathComponent;
struct SolidFillComponent;
struct StrokeComponent;
class Scene;

enum class ChangeOrderType
{
	ToFront,
	ToBack,
	ToForward,
	ToBackward
};

class Entity
{
public:
	Entity() = default;
	Entity(Scene* scene, uint32_t id);
	Entity(Scene* scene);
	virtual ~Entity() = default;

	void moveByDelta(const Vec2& delta);
	void hide();
	void show();
	bool isHidden() const
	{
		return mIsHide;
	}

	void update();
	void updateTransform();
	void updateShapePath();
	void updateShapeAtt();

	template <class T>
	T& getComponent();

	template <typename T, typename... Args>
	T& addComponent(Args&&... args);

	template <typename T>
	bool hasComponent();

	template <typename T>
	void removeComponent();

	template <typename T>
	bool tryRemoveComponent();

	template <typename T, typename... Args>
	bool tryAddComponent(Args&&... args);

	const EntityID getId();
	const bool isNull() const;

	void changeDrawOrder(ChangeOrderType changeOrderType);

	bool operator==(const Entity& rhs)
	{
		return (rhs.rScene == this->rScene) && (rhs.mHandle == this->mHandle);
	}

	entt::entity mHandle{entt::null};

private:
	core::Scene* rScene{nullptr};
	bool mIsHide{false};
};

}	 // namespace core

#endif
