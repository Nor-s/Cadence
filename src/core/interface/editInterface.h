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
	EDIT_API Edit_Result UpdateEntityEnd(ENTITY_ID id);

	EDIT_API void RemoveSelection();

#ifdef __cplusplus
}	 // extern "C"
#endif

#endif
