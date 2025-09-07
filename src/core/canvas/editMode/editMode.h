#ifndef _CORE_CANVAS_EDITMODE_EDITMODE_H_
#define _CORE_CANVAS_EDITMODE_EDITMODE_H_

#include "thorvg.h"
#include "core/input/input.h"

namespace core
{
class Scene;
class AnimationCreatorCanvas;
class InputController;

enum class EditModeType
{
	NONE,
	PICK,
	ADD_SQUARE,
	ADD_ELLIPSE,
	ADD_POLYGON,
	ADD_STAR,
	ADD_PEN_PATH,
	EDIT_PEN_PATH
};

class EditMode
{
public:
	virtual ~EditMode() = default;
	virtual void onUpdate()
	{
	}
	virtual bool onDoubleClickLeftMouse(const InputValue& inputValue)
	{
		return false;
	}
	virtual bool onStarClickLefttMouse(const InputValue& inputValue) = 0;
	virtual bool onDragLeftMouse(const InputValue& inputValue) = 0;
	virtual bool onMoveMouse(const InputValue& inputValue)
	{
		return false;
	}
	virtual bool onEndLeftMouse(const InputValue& inputValue) = 0;
	virtual bool onInputAttach(const InputValue& inputValue)
	{
		return false;
	};
	virtual bool onInputDetach(const InputValue& inputValue) = 0;
};

}	 // namespace core

#endif