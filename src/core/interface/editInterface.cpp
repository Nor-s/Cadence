#include "editInterface.h"

#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "canvas/animationCreatorCanvas.h"

#include "animation/animator.h"
#include "editHelper.h"

using namespace core;

CANVAS_ptr gCurrentCanvas{nullptr};
AnimationCreatorCanvas* gCurrentAnimCanvas{nullptr};

#ifdef __cplusplus
extern "C"
{
#endif

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
		auto* scene = FindScene(id);
		if (scene == nullptr)
			return 0;

		auto entity = scene->createRectFillLayer(Vec2{minX, minY}, Vec2{w, h});
		return entity.getComponent<IDComponent>().id;
	}

	EDIT_API ENTITY_ID CreateElipsePathEntity(SCENE_ID id, float minX, float minY, float w, float h)
	{
		auto* scene = FindScene(id);
		if (scene == nullptr)
			return 0;

		auto entity = scene->createEllipseFillLayer(Vec2{minX, minY}, Vec2{w, h});
		return entity.getComponent<IDComponent>().id;
	}
	EDIT_API ENTITY_ID CreatePolygonPathEntity(SCENE_ID id, float minX, float minY, float w, float h)
	{
		auto* scene = FindScene(id);
		if (scene == nullptr)
			return 0;

		auto entity = scene->createPolygonFillLayer(Vec2{minX, minY}, Vec2{w, h});
		return entity.getComponent<IDComponent>().id;
	}
	EDIT_API ENTITY_ID CreateStarPathEntity(SCENE_ID id, float minX, float minY, float w, float h)
	{
		auto* scene = FindScene(id);
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
			entity.addComponent<TransformKeyframeComponent>();
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
		}

		entity.setDirty(core::Dirty::Type::Transform);

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
		}

		entity.setDirty(core::Dirty::Type::Transform);

		return EDIT_RESULT_SUCCESS;
	}

	EDIT_API Edit_Result RemoveFillComponent(ENTITY_ID id)
	{
		auto entity = Scene::FindEntity(id);
		if (entity.tryRemoveComponent<SolidFillComponent>())
		{
			entity.setDirty(Dirty::Type::Path);
			return EDIT_RESULT_SUCCESS;
		}
		return EDIT_RESULT_FAIL;
	}

	EDIT_API Edit_Result RemoveStrokeComponent(ENTITY_ID id)
	{
		auto entity = Scene::FindEntity(id);
		if (entity.tryRemoveComponent<StrokeComponent>())
		{
			entity.setDirty(Dirty::Type::Path);
			return EDIT_RESULT_SUCCESS;
		}
		return EDIT_RESULT_FAIL;
	}

	EDIT_API Edit_Result AddFillComponent(ENTITY_ID id)
	{
		auto entity = Scene::FindEntity(id);
		if (entity.tryAddComponent<SolidFillComponent>())
		{
			entity.setDirty(Dirty::Type::Fill);
			return EDIT_RESULT_SUCCESS;
		}
		return EDIT_RESULT_FAIL;
	}

	EDIT_API Edit_Result AddStrokeComponent(ENTITY_ID id)
	{
		auto entity = Scene::FindEntity(id);
		if (entity.tryAddComponent<StrokeComponent>())
		{
			entity.setDirty(Dirty::Type::Stroke);
			return EDIT_RESULT_SUCCESS;
		}
		return EDIT_RESULT_FAIL;
	}
	// Rect
	EDIT_API Edit_Result
	UpdateEntityRectPathRadiusCurrentFrame(ENTITY_ID id, int pathIndex, float radius, bool isEnd, bool isRemove)
	{
		return SetProp<RectPath>(id, pathIndex, radius, &RectPath::radius, &RectPath::radiusKeyframes, isEnd);
	}
	EDIT_API Edit_Result
	UpdateEntityRectPathPositionCurrentFrame(ENTITY_ID id, int pathIndex, float x, float y, bool isEnd, bool isRemove)
	{
		return SetProp<RectPath>(id, pathIndex, Vec2{x, y}, &RectPath::position, &RectPath::positionKeyframes, isEnd);
	}
	EDIT_API Edit_Result
	UpdateEntityRectPathScaleCurrentFrame(ENTITY_ID id, int pathIndex, float sx, float sy, bool isEnd, bool isRemove)
	{
		return SetProp<RectPath>(id, pathIndex, Vec2{sx, sy}, &RectPath::scale, &RectPath::scaleKeyframes, isEnd);
	}

	// Ellipse
	EDIT_API Edit_Result
	UpdateEntityElipsePathPositionCurrentFrame(ENTITY_ID id, int pathIndex, float x, float y, bool isEnd, bool isRemove)
	{
		return SetProp<EllipsePath>(id, pathIndex, Vec2{x, y}, &EllipsePath::position, &EllipsePath::positionKeyframes,
									isEnd);
	}
	EDIT_API Edit_Result
	UpdateEntityElipsePathScaleCurrentFrame(ENTITY_ID id, int pathIndex, float sx, float sy, bool isEnd, bool isRemove)
	{
		return SetProp<EllipsePath>(id, pathIndex, Vec2{sx, sy}, &EllipsePath::scale, &EllipsePath::scaleKeyframes,
									isEnd);
	}

	// Polygon
	EDIT_API Edit_Result
	UpdateEntityPolygonPathPointsCurrentFrame(ENTITY_ID id, int pathIndex, int points, bool isEnd, bool isRemove)
	{
		return SetProp<PolygonPath>(id, pathIndex, points, &PolygonPath::points, &PolygonPath::pointsKeyframes, isEnd);
	}
	EDIT_API Edit_Result
	UpdateEntityPolygonPathRotationCurrentFrame(ENTITY_ID id, int pathIndex, float rotation, bool isEnd, bool isRemove)
	{
		return SetProp<PolygonPath>(id, pathIndex, rotation, &PolygonPath::rotation, &PolygonPath::rotationKeyframes,
									isEnd);
	}
	EDIT_API Edit_Result UpdateEntityPolygonPathOuterRadiusCurrentFrame(ENTITY_ID id,
																		int pathIndex,
																		float outerRadius,
																		bool isEnd,
																		bool isRemove)
	{
		return SetProp<PolygonPath>(id, pathIndex, outerRadius, &PolygonPath::outerRadius,
									&PolygonPath::outerRadiusKeyframes, isEnd);
	}
	EDIT_API Edit_Result UpdateEntityPolygonPathPositionCurrentFrame(ENTITY_ID id,
																	 int pathIndex,
																	 float x,
																	 float y,
																	 bool isEnd,
																	 bool isRemove)
	{
		return SetProp<PolygonPath>(id, pathIndex, Vec2{x, y}, &PolygonPath::position, &PolygonPath::positionKeyframes,
									isEnd);
	}

	// Star Polygon
	EDIT_API Edit_Result
	UpdateEntityStarPolygonPathPointsCurrentFrame(ENTITY_ID id, int pathIndex, int points, bool isEnd, bool isRemove)
	{
		return SetProp<StarPolygonPath>(id, pathIndex, points, &StarPolygonPath::points,
										&StarPolygonPath::pointsKeyframes, isEnd);
	}
	EDIT_API Edit_Result UpdateEntityStarPolygonPathRotationCurrentFrame(ENTITY_ID id,
																		 int pathIndex,
																		 float rotation,
																		 bool isEnd,
																		 bool isRemove)
	{
		return SetProp<StarPolygonPath>(id, pathIndex, rotation, &StarPolygonPath::rotation,
										&StarPolygonPath::rotationKeyframes, isEnd);
	}
	EDIT_API Edit_Result UpdateEntityStarPolygonPathOuterRadiusCurrentFrame(ENTITY_ID id,
																			int pathIndex,
																			float outerRadius,
																			bool isEnd,
																			bool isRemove)
	{
		return SetProp<StarPolygonPath>(id, pathIndex, outerRadius, &StarPolygonPath::outerRadius,
										&StarPolygonPath::outerRadiusKeyframes, isEnd);
	}
	EDIT_API Edit_Result UpdateEntityStarPolygonPathInnerRadiusCurrentFrame(ENTITY_ID id,
																			int pathIndex,
																			float innerRadius,
																			bool isEnd,
																			bool isRemove)
	{
		return SetProp<StarPolygonPath>(id, pathIndex, innerRadius, &StarPolygonPath::innerRadius,
										&StarPolygonPath::innerRadiusKeyframes, isEnd);
	}
	EDIT_API Edit_Result UpdateEntityStarPolygonPathPositionCurrentFrame(ENTITY_ID id,
																		 int pathIndex,
																		 float x,
																		 float y,
																		 bool isEnd,
																		 bool isRemove)
	{
		return SetProp<StarPolygonPath>(id, pathIndex, Vec2{x, y}, &StarPolygonPath::position,
										&StarPolygonPath::positionKeyframes, isEnd);
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
			trk.positionKeyframes.add(frameNo, tr.localPosition);
		}

		entity.setDirty(core::Dirty::Type::Transform);

		return EDIT_RESULT_SUCCESS;
	}

	EDIT_API Edit_Result
	UpdateEntitySolidFillColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd, bool isRemove)
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
		}
		entity.setDirty(core::Dirty::Type::Fill);

		return EDIT_RESULT_SUCCESS;
	}

	EDIT_API Edit_Result UpdateEntitySolidFillAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd, bool isRemove)
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
		}
		entity.setDirty(core::Dirty::Type::Fill);
		return EDIT_RESULT_SUCCESS;
	}

	EDIT_API Edit_Result UpdateEntityStrokeWidthCurrentFrame(ENTITY_ID id, float w, bool isEnd, bool isRemove)
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
		}
		entity.setDirty(core::Dirty::Type::Stroke);
		return EDIT_RESULT_SUCCESS;
	}
	EDIT_API Edit_Result
	UpdateEntityStrokeColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd, bool isRemove)
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
		}
		entity.setDirty(core::Dirty::Type::Stroke);
		return EDIT_RESULT_SUCCESS;
	}
	EDIT_API Edit_Result UpdateEntityStrokeAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd, bool isRemove)
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
		}
		entity.setDirty(core::Dirty::Type::Stroke);
		return EDIT_RESULT_SUCCESS;
	}

	EDIT_API void UpdateEntityEnd(ENTITY_ID id)
	{
		return;
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
#ifdef __cplusplus
}	 // extern "C"
#endif
