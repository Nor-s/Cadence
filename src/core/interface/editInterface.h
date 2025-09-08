#ifndef _CORE_INTERFACE_EDIT_INTERFACE_H_
#define _CORE_INTERFACE_EDIT_INTERFACE_H_

#define EDIT_API

#ifdef __cplusplus
extern "C"
{
#endif

	typedef int ENTITY_ID;
	typedef int SCENE_ID;
	typedef int CANVAS_ID;
	typedef void* CANVAS_ptr;

	typedef enum
	{
		EDIT_RESULT_SUCCESS = 0,
		EDIT_RESULT_INVALID_ENTITY = 1,
		EDIT_RESULT_FAIL = 2,
		EDIT_RESULT_UNKNOWN = 255
	} Edit_Result;

	typedef struct
	{
		float positionX;
		float positionY;
		float scaleX;
		float scaleY;
		float rotation;
	} UpdateEntityTransform;

	/**
	 * temp code
	 */
	EDIT_API void FocusCurrentCanvas(CANVAS_ptr canvs);
	EDIT_API CANVAS_ptr GetCurrentCanvas();
	EDIT_API CANVAS_ptr GetCurrentAnimCanvas();

	/***
	 * todo: UNDO/REDO
	 */
	EDIT_API ENTITY_ID CreateRectPathEntity(SCENE_ID id, float minX, float minY, float w, float h);
	EDIT_API ENTITY_ID CreateElipsePathEntity(SCENE_ID id, float minX, float minY, float w, float h);
	EDIT_API ENTITY_ID CreatePolygonPathEntity(SCENE_ID id, float minX, float minY, float w, float h);
	EDIT_API ENTITY_ID CreateStarPathEntity(SCENE_ID id, float minX, float minY, float w, float h);

	EDIT_API Edit_Result UpdateEntityTransformCurrentFrame(ENTITY_ID id, UpdateEntityTransform* transform, bool isEnd);
	EDIT_API Edit_Result UpdateEntityRotationCurrentFrame(ENTITY_ID id, float x, bool isEnd);
	EDIT_API Edit_Result UpdateEntityPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd);
	EDIT_API Edit_Result UpdateEntityScaleCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd);
	EDIT_API Edit_Result UpdateEntityDeltaPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd);
	EDIT_API Edit_Result UpdateEntityDeltaRotationCurrentFrame(ENTITY_ID id, float x, bool isEnd);

	// remove component
	EDIT_API Edit_Result RemoveFillComponent(ENTITY_ID id);
	EDIT_API Edit_Result RemoveStrokeComponent(ENTITY_ID id);

	// add component
	EDIT_API Edit_Result AddFillComponent(ENTITY_ID id);
	EDIT_API Edit_Result AddStrokeComponent(ENTITY_ID id);

	// Rect
	EDIT_API Edit_Result UpdateEntityRectPathRadiusCurrentFrame(ENTITY_ID id, float radius, bool isEnd);
	EDIT_API Edit_Result UpdateEntityRectPathPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd);
	EDIT_API Edit_Result UpdateEntityRectPathScaleCurrentFrame(ENTITY_ID id, float sx, float sy, bool isEnd);

	// Elipse
	EDIT_API Edit_Result UpdateEntityElipsePathPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd);
	EDIT_API Edit_Result UpdateEntityElipsePathScaleCurrentFrame(ENTITY_ID id, float sx, float sy, bool isEnd);

	// Polygon
	EDIT_API Edit_Result UpdateEntityPolygonPathPointsCurrentFrame(ENTITY_ID id, int points, bool isEnd);
	EDIT_API Edit_Result UpdateEntityPolygonPathRotationCurrentFrame(ENTITY_ID id, float rotation, bool isEnd);
	EDIT_API Edit_Result UpdateEntityPolygonPathOuterRadiusCurrentFrame(ENTITY_ID id, float outerRadius, bool isEnd);
	EDIT_API Edit_Result UpdateEntityPolygonPathPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd);

	// StarPolygon
	EDIT_API Edit_Result UpdateEntityStarPolygonPathPointsCurrentFrame(ENTITY_ID id, int points, bool isEnd);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathRotationCurrentFrame(ENTITY_ID id, float rotation, bool isEnd);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathOuterRadiusCurrentFrame(ENTITY_ID id,
																			float outerRadius,
																			bool isEnd);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathInnerRadiusCurrentFrame(ENTITY_ID id,
																			float innerRadius,
																			bool isEnd);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathPositionCurrentFrame(ENTITY_ID id, float x, float y, bool isEnd);

	// solid fill
	EDIT_API Edit_Result UpdateEntitySolidFillColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd);
	EDIT_API Edit_Result UpdateEntitySolidFillAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd);

	// stroke
	EDIT_API Edit_Result UpdateEntityStrokeWidthCurrentFrame(ENTITY_ID id, float w, bool isEnd);
	EDIT_API Edit_Result UpdateEntityStrokeColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd);
	EDIT_API Edit_Result UpdateEntityStrokeAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd);

	EDIT_API Edit_Result UpdateEntityEnd(ENTITY_ID id);

	EDIT_API void RemoveSelection();

#ifdef __cplusplus
}	 // extern "C"
#endif

#endif
