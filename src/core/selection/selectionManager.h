#ifndef _CORE_SELECTION_MANAGER_H_
#define _CORE_SELECTION_MANAGER_H_

#include "scene/entity.h"
#include <unordered_map>

namespace core
{
class AnimationCreatorCanvas;
class SelectionManager
{
public:
	static void Select(AnimationCreatorCanvas* canvas, Entity entity);
    static void Hover(AnimationCreatorCanvas* canvas, Entity entity);
	static void Push(AnimationCreatorCanvas* canvas, Entity entity);
	static void Clear(AnimationCreatorCanvas* canvas);
	static void Update();
	static void Update(AnimationCreatorCanvas* canvas);

private:
	static SelectionManager& Get();
	SelectionManager() = default;

private:
	std::unordered_map<AnimationCreatorCanvas*, std::vector<Entity> > mSelectList;
	std::unordered_map<AnimationCreatorCanvas*, Entity> mHover;
};

}	 // namespace core

#endif