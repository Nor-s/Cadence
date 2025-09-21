#include "entity.h"
#pragma once

namespace core
{

template <class T>
T& Entity::getComponent() const
{
	assert(hasComponent<T>());
	return rScene->mRegistry.get<T>(mHandle);
}

template <typename T, typename... Args>
inline T& Entity::getOrAddComponent(Args&&... args)
{
	if (hasComponent<T>())
	{
		return getComponent<T>();
	}
	return addComponent<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
T& Entity::addComponent(Args&&... args)
{
	assert(!hasComponent<T>());
	return rScene->mRegistry.emplace<T>(mHandle, std::forward<Args>(args)...);
}

template <typename T>
bool Entity::hasComponent() const
{
	assert(mHandle != entt::null);
	return rScene->mRegistry.try_get<T>(mHandle) != nullptr;
}

template <typename T>
void Entity::removeComponent()
{
	assert(hasComponent<T>());
	rScene->mRegistry.remove<T>(mHandle);
}

template <typename T>
bool Entity::tryRemoveComponent()
{
	if (hasComponent<T>())
	{
		RemoveShape<T>(*this);
		removeComponent<T>();
		return true;
	}
	return false;
}

template <typename T, typename... Args>
bool Entity::tryAddComponent(Args&&... args)
{
	if (!hasComponent<T>())
	{
		addComponent<T>(std::forward<Args>(args)...);
		return true;
	}
	return false;
}

template <class T>
inline const T* const Entity::tryGetComponent() const
{
	if (hasComponent<T>())
	{
		return &getComponent<T>();
	}
	return nullptr;
}

template <typename T>
inline T* Entity::findPath(int startIdx)
{
	auto& paths = getComponent<PathListComponent>().paths;
	for (int i = startIdx; i < paths.size(); i++)
	{
		if (PathTag<T>::type == paths[i]->type())
		{
			return static_cast<T*>(paths[i].get());
		}
	}

	return nullptr;
}

template <typename T>
inline T* Entity::getPath(int idx)
{
	auto& paths = getComponent<PathListComponent>().paths;
	if (PathTag<T>::type == paths[idx]->type())
	{
		return static_cast<T*>(paths[idx].get());
	}
	return nullptr;
}

}	 // namespace core