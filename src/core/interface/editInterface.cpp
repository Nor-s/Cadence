#include "editInterface.h"

#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "canvas/animationCreatorCanvas.h"

#include "animation/animator.h"

using namespace core;

namespace helper
{

Scene* FindScene(SCENE_ID id)
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

}	 // namespace helper

CANVAS_ptr gCurrentCanvas{nullptr};
AnimationCreatorCanvas* gCurrentAnimCanvas{nullptr};

EDIT_API void FocusCurrentCanvas(CANVAS_ptr canvas)
{
	if (canvas != nullptr)
		gCurrentCanvas = canvas;

	auto* rawcanvas = static_cast<CanvasWrapper*>(canvas);
	if (rawcanvas->type() == CanvasType::LottieCreator)
	{
		gCurrentAnimCanvas = static_cast<AnimationCreatorCanvas*>(canvas);
	}
}

CANVAS_ptr GetCurrentAnimCanvas()
{
	return gCurrentAnimCanvas;
}

EDIT_API ENTITY_ID CreateRectPathEntity(SCENE_ID id, float minX, float minY, float w, float h)
{
	auto* scene = helper::FindScene(id);
	if (scene == nullptr)
		return 0;

	auto entity = scene->createRectFillLayer(Vec2{minX, minY}, Vec2{w, h});
	return entity.getComponent<IDComponent>().id;
}

EDIT_API ENTITY_ID CreateElipsePathEntity(SCENE_ID id, float minX, float minY, float w, float h)
{
	auto* scene = helper::FindScene(id);
	if (scene == nullptr)
		return 0;

	auto entity = scene->createEllipseFillLayer(Vec2{minX, minY}, Vec2{w, h});
	return entity.getComponent<IDComponent>().id;
}
EDIT_API ENTITY_ID CreatePolygonPathEntity(SCENE_ID id, float minX, float minY, float w, float h)
{
	auto* scene = helper::FindScene(id);
	if (scene == nullptr)
		return 0;

	auto entity = scene->createPolygonFillLayer(Vec2{minX, minY}, Vec2{w, h});
	return entity.getComponent<IDComponent>().id;
}
EDIT_API ENTITY_ID CreateStarPathEntity(SCENE_ID id, float minX, float minY, float w, float h)
{
	auto* scene = helper::FindScene(id);
	if (scene == nullptr)
		return 0;

	auto entity = scene->createStarFillLayer(Vec2{minX, minY}, Vec2{w, h});
	return entity.getComponent<IDComponent>().id;
}

EDIT_API Edit_Result UpdateEntityTransformCurrentFrame(ENTITY_ID id, UpdateEntityTransform* transform, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull())
		return EDIT_RESULT_INVALID_ENTITY;

	LOG_CRITICAL("TODO: UpdateEntityTransformCurrentFrame");

	return EDIT_RESULT_SUCCESS;
}

EDIT_API Edit_Result UpdateEntityRotationCurrentFrame(ENTITY_ID id, float x, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull())
		return EDIT_RESULT_INVALID_ENTITY;

	LOG_CRITICAL("TODO: UpdateEntityRotationCurrentFrame");

	return EDIT_RESULT_SUCCESS;
}

EDIT_API Edit_Result UpdateEntityPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull())
		return EDIT_RESULT_INVALID_ENTITY;

	LOG_CRITICAL("TODO: UpdateEntityPositionCurrentFrame");

	return EDIT_RESULT_SUCCESS;
}

void AddTransformKeyframe(Entity entity)
{
	if (entity.hasComponent<TransformKeyframeComponent>() == false)
	{
		// auto& tr = entity.getComponent<TransformComponent>();
		entity.addComponent<TransformKeyframeComponent>();

		// trk.positionKeyframes.add(0, tr.localCenterPosition);
		// trk.scaleKeyframes.add(0, tr.scale);
		// trk.rotationKeyframes.add(0, tr.rotation);
	}
}

EDIT_API Edit_Result UpdateEntityScaleCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull())
		return EDIT_RESULT_INVALID_ENTITY;

	auto& tr = entity.getComponent<TransformComponent>();
	tr.scale.x = x;
	tr.scale.y = y;

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		AddTransformKeyframe(entity);
		auto& trk = entity.getComponent<TransformKeyframeComponent>();
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		trk.scaleKeyframes.add(frameNo, tr.scale);
		trk.scaleKeyframes.currentValue = tr.scale;
	}

	return EDIT_RESULT_SUCCESS;
}

EDIT_API Edit_Result UpdateEntityDeltaRotationCurrentFrame(ENTITY_ID id, float x, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull())
		return EDIT_RESULT_INVALID_ENTITY;

	auto& tr = entity.getComponent<TransformComponent>();
	tr.rotation += x;

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		AddTransformKeyframe(entity);
		auto& trk = entity.getComponent<TransformKeyframeComponent>();
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		trk.rotationKeyframes.add(frameNo, tr.rotation);
		trk.rotationKeyframes.currentValue = tr.rotation;
	}

	return EDIT_RESULT_SUCCESS;
}

EDIT_API Edit_Result RemoveFillComponent(ENTITY_ID id)
{
	auto entity = Scene::FindEntity(id);
	if (entity.tryRemoveComponent<SolidFillComponent>())
	{
		return EDIT_RESULT_SUCCESS;
	}
	return EDIT_RESULT_FAIL;
}

EDIT_API Edit_Result RemoveStrokeComponent(ENTITY_ID id)
{
	auto entity = Scene::FindEntity(id);
	if (entity.tryRemoveComponent<StrokeComponent>())
	{
		return EDIT_RESULT_SUCCESS;
	}
	return EDIT_RESULT_FAIL;
}

EDIT_API Edit_Result AddFillComponent(ENTITY_ID id)
{
	auto entity = Scene::FindEntity(id);
	if (entity.tryAddComponent<SolidFillComponent>())
	{
		return EDIT_RESULT_SUCCESS;
	}
	return EDIT_RESULT_FAIL;
}

EDIT_API Edit_Result AddStrokeComponent(ENTITY_ID id)
{
	auto entity = Scene::FindEntity(id);
	if (entity.tryAddComponent<StrokeComponent>())
	{
		return EDIT_RESULT_SUCCESS;
	}
	return EDIT_RESULT_FAIL;
}

EDIT_API Edit_Result UpdateEntityDeltaPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull())
		return EDIT_RESULT_INVALID_ENTITY;

	entity.moveByDelta({x, y});

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		AddTransformKeyframe(entity);
		auto& trk = entity.getComponent<TransformKeyframeComponent>();
		auto& tr = entity.getComponent<TransformComponent>();
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		trk.positionKeyframes.add(frameNo, tr.localCenterPosition);
		trk.positionKeyframes.currentValue = tr.localCenterPosition;
	}

	return EDIT_RESULT_SUCCESS;
}

EDIT_API Edit_Result UpdateEntitySolidFillColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull() || !entity.hasComponent<SolidFillComponent>())
		return EDIT_RESULT_INVALID_ENTITY;

	auto& solidFill = entity.getComponent<SolidFillComponent>();
	solidFill.color.r = r;
	solidFill.color.g = g;
	solidFill.color.b = b;

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		solidFill.colorKeyframe.add(frameNo, solidFill.color);
		solidFill.colorKeyframe.currentValue = solidFill.color;
	}
}

EDIT_API Edit_Result UpdateEntitySolidFillAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull() || !entity.hasComponent<SolidFillComponent>())
		return EDIT_RESULT_INVALID_ENTITY;

	auto& solidFill = entity.getComponent<SolidFillComponent>();
	solidFill.alpha = a;

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		solidFill.alphaKeyframe.add(frameNo, solidFill.alpha);
		solidFill.alphaKeyframe.currentValue = solidFill.alpha;
	}
}

EDIT_API Edit_Result UpdateEntityStrokeWidthCurrentFrame(ENTITY_ID id, float w, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull() || !entity.hasComponent<StrokeComponent>())
		return EDIT_RESULT_INVALID_ENTITY;

	auto& stroke = entity.getComponent<StrokeComponent>();
	stroke.width = w;

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		stroke.widthKeyframe.add(frameNo, stroke.width);
		stroke.widthKeyframe.currentValue = stroke.width;
	}
}
EDIT_API Edit_Result UpdateEntityStrokeColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull() || !entity.hasComponent<StrokeComponent>())
		return EDIT_RESULT_INVALID_ENTITY;

	auto& stroke = entity.getComponent<StrokeComponent>();
	stroke.color.r = r;
	stroke.color.g = g;
	stroke.color.b = b;

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		stroke.colorKeyframe.add(frameNo, stroke.color);
		stroke.colorKeyframe.currentValue = stroke.color;
	}
}
EDIT_API Edit_Result UpdateEntityStrokeAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd)
{
	auto entity = Scene::FindEntity(id);
	if (entity.isNull() || !entity.hasComponent<StrokeComponent>())
		return EDIT_RESULT_INVALID_ENTITY;

	auto& stroke = entity.getComponent<StrokeComponent>();
	stroke.alpha = a;

	if (gCurrentAnimCanvas && gCurrentAnimCanvas->mAnimator->mIsStop)
	{
		const auto frameNo = gCurrentAnimCanvas->mAnimator->mCurrentFrameNo;
		stroke.alphaKeyframe.add(frameNo, stroke.alpha);
		stroke.alphaKeyframe.currentValue = stroke.alpha;
	}
}

// for undo
EDIT_API Edit_Result UpdateEntityEnd(ENTITY_ID id)
{
	return EDIT_RESULT_SUCCESS;
}

EDIT_API void RemoveSelection()
{
	if (gCurrentAnimCanvas)
	{
		auto entities = gCurrentAnimCanvas->mControlScene->findByComponent<BBoxControlComponent>();
		for (auto& entity : entities)
		{
			gCurrentAnimCanvas->mControlScene->destroyEntity(entity);
		}
	}
}