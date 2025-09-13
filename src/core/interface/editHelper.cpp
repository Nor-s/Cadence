#include "editHelper.h"

#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "canvas/animationCreatorCanvas.h"

#include "animation/animator.h"
#include "editHelper.h"

using namespace core;

extern CANVAS_ptr gCurrentCanvas;
extern AnimationCreatorCanvas* gCurrentAnimCanvas;

core::Scene* FindScene(SCENE_ID id)
{
	auto entity = core::Scene::FindEntity(id);

	if (entity.mHandle != entt::null && entity.hasComponent<SceneComponent>())
	{
		auto& scene = entity.getComponent<SceneComponent>();

		if (scene.scene == nullptr)
			return nullptr;
		return scene.scene;
	}

	return nullptr;
}
int CurrentFrameNo()
{
	return (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator) ? (int) gCurrentAnimCanvas->mAnimator->mCurrentFrameNo
																 : 0;
}
bool IsPlaybackStopped()
{
	return (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator) ? gCurrentAnimCanvas->mAnimator->mIsStop : true;
}
core::Entity GetEntity(ENTITY_ID id)
{
	return core::Scene::FindEntity(id);
}
bool FetchPath(core::Entity e, int idx, IPath*& out)
{
	if (e.isNull() || !e.hasComponent<PathListComponent>())
		return false;
	auto& pc = e.getComponent<PathListComponent>();
	if (idx < 0 || idx >= (int) pc.paths.size())
		return false;
	out = pc.paths[idx].get();
	return out != nullptr;
}
bool ShouldAddKeyframe()
{
	return IsPlaybackStopped();
}
