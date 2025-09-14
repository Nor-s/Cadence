#ifndef _CORE_SCENE_NSENTITY_H_
#define _CORE_SCENE_NSENTITY_H_

#include "../common/common.h"
#include "component/path.h"
#include <vector>
#include <memory>

#include <entt/entt.hpp>

namespace core
{
using EntityID = uint32_t;

struct ShapeComponent;
struct SolidFillComponent;
struct StrokeComponent;
class Scene;

struct Dirty
{
	enum class Type : uint32_t
	{
		None = 0,
		Path = 1 << 0,
		Transform = 1 << 1,
		Fill = 1 << 2,
		Stroke = 1 << 3,
		All = 0xFFFF
	};
	Type mask{0};

	Dirty& operator|=(Dirty::Type type)
	{
		mask = static_cast<Dirty::Type>(static_cast<uint32_t>(mask) | static_cast<uint32_t>(type));
		return *this;
	}
};

inline static bool HasDirty(Dirty& dirty, Dirty::Type type)
{
	return (static_cast<uint32_t>(dirty.mask) & static_cast<uint32_t>(type)) != 0;
}

inline static Dirty::Type operator|(Dirty& dirty, Dirty::Type type)
{
	return static_cast<Dirty::Type>(static_cast<uint32_t>(dirty.mask) | static_cast<uint32_t>(type));
}

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

	void setScaleByDelta(const Vec2& delta);
	void moveByDelta(const Vec2& delta);
	void move(const Vec2& pos);
	void hide();
	void show();
	bool isHidden() const
	{
		return mIsHide;
	}
	void setDirty(Dirty::Type dirtyType = Dirty::Type::All);

	void update();
	void updateTransform();
	void updateShapePath();
	bool updateShapePath(float keyframeNo);
	void updateShapeAtt();

	template <class T>
	T& getComponent() const;

	template <class T, typename... Args>
	T& getOrAddComponent(Args&&... args);

	template <typename T, typename... Args>
	T& addComponent(Args&&... args);

	template <typename T>
	bool hasComponent() const;

	template <typename T>
	void removeComponent();

	template <typename T>
	bool tryRemoveComponent();

	template <typename T, typename... Args>
	bool tryAddComponent(Args&&... args);

	template <class T>
	const T* const tryGetComponent() const;

	template <typename T>
	T* findPath(int startIdx = 0);

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
