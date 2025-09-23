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
	} Edit_Transform;

	typedef enum
	{
		EDIT_PathPointType_LineTo = 0,
		EDIT_PathPointType_MoveTo = 1,
		EDIT_PathPointType_CubicTo = 2,
		EDIT_PathPointType_Close = 3
	} Edit_PathPointType;

	typedef struct Edit_PathPoint
	{
		float localPosition[2] = {0.0f, 0.0f};
		float leftControlRelPosition[2] = {0.0f, 0.0f};
		float rightControlRelPosition[2] = {0.0f, 0.0f};
		Edit_PathPointType type = EDIT_PathPointType_LineTo;
	} Edit_PathPoint;

	typedef enum
	{
		EDIT_MODE_NONE = 0,
		EDIT_MODE_PICK = 1,
		EDIT_MODE_ADD_SQUARE = 2,
		EDIT_MODE_ADD_ELLIPSE = 3,
		EDIT_MODE_ADD_POLYGON = 4,
		EDIT_MODE_ADD_STAR = 5,
		EDIT_MODE_ADD_PEN_PATH = 6,
		EDIT_MODE_EDIT_PATH = 7,
	} Edit_Mode;

	/**
	 * temp code
	 */
	EDIT_API void FocusCurrentCanvas(CANVAS_ptr canvs);
	EDIT_API CANVAS_ptr GetCurrentAnimCanvas();
	EDIT_API Edit_Mode GetCurrentEditMode(CANVAS_ptr canvas);
	EDIT_API void SetEditMode(CANVAS_ptr canvas, Edit_Mode editMode);
	EDIT_API void SetPathEditMode(CANVAS_ptr canvas, ENTITY_ID id, int pathIdx);

	/***
	 * todo: UNDO/REDO
	 */
	EDIT_API ENTITY_ID CreateRectPathEntity(SCENE_ID id, float minX, float minY, float w, float h);
	EDIT_API ENTITY_ID CreateElipsePathEntity(SCENE_ID id, float minX, float minY, float w, float h);
	EDIT_API ENTITY_ID CreatePolygonPathEntity(SCENE_ID id, float minX, float minY, float w, float h);
	EDIT_API ENTITY_ID CreateStarPathEntity(SCENE_ID id, float minX, float minY, float w, float h);

	EDIT_API Edit_Result UpdateEntityTransformCurrentFrame(ENTITY_ID id, Edit_Transform* transform, bool isEnd);
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

	EDIT_API void ClearSelection(CANVAS_ptr canvas);

	// path edit
	EDIT_API Edit_Result Internal_Path_AddPathPoint(ENTITY_ID id,
													int pathIndex,
													Edit_PathPoint* pathPoint,
													bool isAddMode,
													int pointIndex = -1);
	EDIT_API Edit_Result Internal_Path_UpdatePathPoint(ENTITY_ID id,
													   int pathIndex,
													   Edit_PathPoint* pathPoint,
													   bool isAddMode,
													   int pointIndex);

#ifdef __cplusplus
}	 // extern "C"
#endif

#endif
