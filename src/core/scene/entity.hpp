#pragma once

namespace core
{

template <class T>
T& Entity::getComponent()
{
	assert(hasComponent<T>());
	return rScene->mRegistry.get<T>(mHandle);
}

template <typename T, typename... Args>
T& Entity::addComponent(Args&&... args)
{
	assert(!hasComponent<T>());
	return rScene->mRegistry.emplace<T>(mHandle, std::forward<Args>(args)...);
}

template <typename T>
bool Entity::hasComponent()
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

}	 // namespace core