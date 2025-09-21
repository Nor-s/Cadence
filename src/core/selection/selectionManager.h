#ifndef _CORE_SELECTION_MANAGER_H_
#define _CORE_SELECTION_MANAGER_H_

#include "scene/entity.h"
#include <unordered_map>

namespace core
{
class AnimationCreatorCanvas;
class SelectionManager
{
	struct EditPathInfo
	{
		Entity entity;
		int pathIndex;
	};

public:
	static void Select(AnimationCreatorCanvas* canvas, Entity entity);
	static void Hover(AnimationCreatorCanvas* canvas, Entity entity);
	static void Push(AnimationCreatorCanvas* canvas, Entity entity);
	static void Clear(AnimationCreatorCanvas* canvas);
	static void Update();
	static void Update(AnimationCreatorCanvas* canvas);
	static bool IsSelected(AnimationCreatorCanvas* canvas, int id);
	static bool EditPath(AnimationCreatorCanvas* canvas, Entity entity, int pathIdx);

private:
	static SelectionManager& Get();
	SelectionManager() = default;

	void updateSelect(AnimationCreatorCanvas* canvas);
	void updateHover(AnimationCreatorCanvas* canvas);

private:
	std::unordered_map<AnimationCreatorCanvas*, std::vector<Entity> > mSelectList;
	std::unordered_map<AnimationCreatorCanvas*, Entity> mHover;
	std::unordered_map<AnimationCreatorCanvas*, int> mEditPath;
};

}	 // namespace core

#endif