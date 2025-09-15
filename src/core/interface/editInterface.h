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
		EDIT_RESULT_INVALID_INDEX = 3,
		EDIT_RESULT_TYPE_MISMATCH = 4,
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

	typedef enum PathProp
	{
		// Rect
		PATH_PROP_RECT_RADIUS,
		PATH_PROP_RECT_POSITION,	// vec2
		PATH_PROP_RECT_SCALE,		// vec2

		// Ellipse
		PATH_PROP_ELLIPSE_POSITION,	   // vec2
		PATH_PROP_ELLIPSE_SCALE,	   // vec2

		// Polygon
		PATH_PROP_POLY_POINTS,			// int
		PATH_PROP_POLY_ROTATION,		// float
		PATH_PROP_POLY_OUTER_RADIUS,	// float
		PATH_PROP_POLY_POSITION,		// vec2

		// Star
		PATH_PROP_STAR_POINTS,			// int
		PATH_PROP_STAR_ROTATION,		// float
		PATH_PROP_STAR_OUTER_RADIUS,	// float
		PATH_PROP_STAR_INNER_RADIUS,	// float
		PATH_PROP_STAR_POSITION,		// vec2
	} PathProp;

	/**
	 * temp code
	 */
	EDIT_API void FocusCurrentCanvas(CANVAS_ptr canvs);
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

	// move path
	EDIT_API void MovePath(ENTITY_ID sourceId, int pathIndex, ENTITY_ID targetId);

	// Rect
	EDIT_API Edit_Result
	UpdateEntityRectPathRadiusCurrentFrame(ENTITY_ID id, int pathIndex, float radius, bool isEnd, bool isRemove);
	EDIT_API Edit_Result
	UpdateEntityRectPathPositionCurrentFrame(ENTITY_ID id, int pathIndex, float x, float y, bool isEnd, bool isRemove);
	EDIT_API Edit_Result
	UpdateEntityRectPathScaleCurrentFrame(ENTITY_ID id, int pathIndex, float sx, float sy, bool isEnd, bool isRemove);

	// Ellipse
	EDIT_API Edit_Result UpdateEntityElipsePathPositionCurrentFrame(ENTITY_ID id,
																	int pathIndex,
																	float x,
																	float y,
																	bool isEnd,
																	bool isRemove);
	EDIT_API Edit_Result
	UpdateEntityElipsePathScaleCurrentFrame(ENTITY_ID id, int pathIndex, float sx, float sy, bool isEnd, bool isRemove);

	// Polygon
	EDIT_API Edit_Result
	UpdateEntityPolygonPathPointsCurrentFrame(ENTITY_ID id, int pathIndex, int points, bool isEnd, bool isRemove);
	EDIT_API Edit_Result
	UpdateEntityPolygonPathRotationCurrentFrame(ENTITY_ID id, int pathIndex, float rotation, bool isEnd, bool isRemove);
	EDIT_API Edit_Result UpdateEntityPolygonPathOuterRadiusCurrentFrame(ENTITY_ID id,
																		int pathIndex,
																		float outerRadius,
																		bool isEnd,
																		bool isRemove);
	EDIT_API Edit_Result UpdateEntityPolygonPathPositionCurrentFrame(ENTITY_ID id,
																	 int pathIndex,
																	 float x,
																	 float y,
																	 bool isEnd,
																	 bool isRemove);

	//  Star Polygon
	EDIT_API Edit_Result
	UpdateEntityStarPolygonPathPointsCurrentFrame(ENTITY_ID id, int pathIndex, int points, bool isEnd, bool isRemove);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathRotationCurrentFrame(ENTITY_ID id,
																		 int pathIndex,
																		 float rotation,
																		 bool isEnd,
																		 bool isRemove);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathOuterRadiusCurrentFrame(ENTITY_ID id,
																			int pathIndex,
																			float outerRadius,
																			bool isEnd,
																			bool isRemove);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathInnerRadiusCurrentFrame(ENTITY_ID id,
																			int pathIndex,
																			float innerRadius,
																			bool isEnd,
																			bool isRemove);
	EDIT_API Edit_Result UpdateEntityStarPolygonPathPositionCurrentFrame(ENTITY_ID id,
																		 int pathIndex,
																		 float x,
																		 float y,
																		 bool isEnd,
																		 bool isRemove);

	// solid fill
	EDIT_API Edit_Result
	UpdateEntitySolidFillColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd, bool isRemove);
	EDIT_API Edit_Result UpdateEntitySolidFillAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd, bool isRemove);

	// stroke
	EDIT_API Edit_Result UpdateEntityStrokeWidthCurrentFrame(ENTITY_ID id, float w, bool isEnd, bool isRemove);
	EDIT_API Edit_Result
	UpdateEntityStrokeColorCurrentFrame(ENTITY_ID id, float r, float g, float b, bool isEnd, bool isRemove);
	EDIT_API Edit_Result UpdateEntityStrokeAlphaCurrentFrame(ENTITY_ID id, float a, bool isEnd, bool isRemove);

	EDIT_API void UpdateEntityEnd(ENTITY_ID id);

	EDIT_API void RemoveSelection();

#ifdef __cplusplus
}	 // extern "C"
#endif

#endif
