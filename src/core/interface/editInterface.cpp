#include "editInterface.h"

#include "scene/scene.h"
#include "scene/component/components.h"
#include "scene/component/uiComponents.h"
#include "canvas/animationCreatorCanvas.h"
#include "canvas/animationCreatorInputController.h"
#include "selection/selectionManager.h"

#include "animation/animator.h"
#include "editHelper.h"
#include <algorithm>

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
		if (rawcanvas->type() == CanvasType::AnimationCreator)
		{
			gCurrentAnimCanvas = static_cast<AnimationCreatorCanvas*>(canvas);
		}
	}

	CANVAS_ptr GetCurrentAnimCanvas()
	{
		return gCurrentAnimCanvas;
	}

	EDIT_API Edit_Mode GetCurrentEditMode(CANVAS_ptr canvas)
	{
		if (canvas)
		{
			auto* rawcanvas = static_cast<CanvasWrapper*>(canvas);
			if (rawcanvas->type() == CanvasType::AnimationCreator)
			{
				auto* animCanvas = static_cast<AnimationCreatorCanvas*>(canvas);
				return static_cast<Edit_Mode>(animCanvas->mInputController->getMode());
			}
		}
		return Edit_Mode::EDIT_MODE_NONE;
	}

	EDIT_API void SetEditMode(CANVAS_ptr canvas, Edit_Mode editMode)
	{
		if (editMode == Edit_Mode::EDIT_MODE_EDIT_PATH)
		{
			assert(true);
			return;
		}

		if (canvas)
		{
			auto* rawcanvas = static_cast<CanvasWrapper*>(canvas);
			if (rawcanvas->type() == CanvasType::AnimationCreator)
			{
				auto* animCanvas = static_cast<AnimationCreatorCanvas*>(canvas);
				SelectionManager::Clear(animCanvas);
				animCanvas->mInputController->setMode(static_cast<EditModeType>(editMode));
			}
		}
	}

	EDIT_API void SetPathEditMode(CANVAS_ptr canvas, ENTITY_ID id, int pathIdx)
	{
		if (canvas)
		{
			auto* rawcanvas = static_cast<CanvasWrapper*>(canvas);
			if (rawcanvas->type() == CanvasType::AnimationCreator)
			{
				auto* animCanvas = static_cast<AnimationCreatorCanvas*>(canvas);
				if (SelectionManager::SetEditPath(animCanvas, GetEntity(id), pathIdx))
				{
					animCanvas->mInputController->setMode(static_cast<EditModeType>(Edit_Mode::EDIT_MODE_EDIT_PATH));
				}
			}
		}
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

	EDIT_API Edit_Result UpdateEntityTransformCurrentFrame(ENTITY_ID id, Edit_Transform* transform, bool isEnd)
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
	EDIT_API void MovePath(ENTITY_ID sourceId, int pathIndex, ENTITY_ID targetId)
	{
		auto sourceEntity = Scene::FindEntity(sourceId);
		auto targetEntity = Scene::FindEntity(targetId);
		if (sourceEntity.isNull() || targetEntity.isNull() || sourceId == targetId)
			return;

		if (sourceEntity.hasComponent<PathListComponent>() && targetEntity.hasComponent<PathListComponent>())
		{
			auto& sourcePathList = sourceEntity.getComponent<PathListComponent>();
			auto& targetPathList = targetEntity.getComponent<PathListComponent>();
			if (pathIndex < 0 || pathIndex >= (int) sourcePathList.paths.size())
				return;

			auto path = std::move(sourcePathList.paths[pathIndex]);
			auto& sourcePath = sourcePathList.paths;
			sourcePath.erase(sourcePathList.paths.begin() + pathIndex);
			targetPathList.paths.push_back(std::move(path));

			sourceEntity.setDirty(Dirty::Type::Path);
			targetEntity.setDirty(Dirty::Type::Path);
		}
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

	EDIT_API void ClearSelection(CANVAS_ptr canvas)
	{
		if (canvas == nullptr)
			return;

		auto* rawcanvas = static_cast<CanvasWrapper*>(canvas);
		if (rawcanvas->type() == CanvasType::AnimationCreator)
		{
			SelectionManager::Clear(static_cast<AnimationCreatorCanvas*>(canvas));
		}
	}

	EDIT_API Edit_Result
	Internal_Path_AddPathPoint(ENTITY_ID id, int pathIndex, Edit_PathPoint* pathPoint, bool isAddMode, int pointIndex)
	{
		auto entity = Scene::FindEntity(id);
		if (entity.isNull())
		{
			return EDIT_RESULT_INVALID_ENTITY;
		}
		if (entity.hasComponent<PathListComponent>() == false)
		{
			return EDIT_RESULT_TYPE_MISMATCH;
		}
		if (pathPoint == nullptr)
		{
			return EDIT_RESULT_FAIL;
		}
		if (pointIndex >= 0)
		{
			// todo: add path point
			assert(true);
		}

		auto* path = entity.getPath<RawPath>(pathIndex);
		if (path == nullptr)
		{
			return EDIT_RESULT_FAIL;
		}

		PathPoint p{
			.localPosition = {pathPoint->localPosition[0], pathPoint->localPosition[1]},
			.deltaLeftControlPosition = {pathPoint->leftControlRelPosition[0], pathPoint->leftControlRelPosition[1]},
			.deltaRightControlPosition = {pathPoint->rightControlRelPosition[0], pathPoint->rightControlRelPosition[1]},
			.type = static_cast<PathPoint::Command>(pathPoint->type)};

		path->path.push_back(p);
		entity.setDirty(Dirty::Type::Path);
	}

	EDIT_API Edit_Result Internal_Path_UpdatePathPoint(ENTITY_ID id,
													   int pathIndex,
													   Edit_PathPoint* pathPoint,
													   bool isAddMode,
													   int pointIndex)
	{
		auto entity = Scene::FindEntity(id);
		if (entity.isNull())
		{
			return EDIT_RESULT_INVALID_ENTITY;
		}
		if (entity.hasComponent<PathListComponent>() == false)
		{
			return EDIT_RESULT_TYPE_MISMATCH;
		}
		if (pathPoint == nullptr)
		{
			return EDIT_RESULT_FAIL;
		}

		auto* path = entity.getPath<RawPath>(pathIndex);

		if (path == nullptr || pointIndex >= path->path.size() || pointIndex < 0)
		{
			return EDIT_RESULT_FAIL;
		}
		auto& p = path->path[pointIndex];

		p.localPosition = {pathPoint->localPosition[0], pathPoint->localPosition[1]};
		p.deltaLeftControlPosition = {pathPoint->leftControlRelPosition[0], pathPoint->leftControlRelPosition[1]};
		p.deltaRightControlPosition = {pathPoint->rightControlRelPosition[0], pathPoint->rightControlRelPosition[1]};
		p.type = static_cast<PathPoint::Command>(pathPoint->type);

		entity.setDirty(Dirty::Type::Path);
	}

#ifdef __cplusplus
}	 // extern "C"
#endif
