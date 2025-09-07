#ifndef _CORE_CANVAS_ANIMATION_CREATOR_INPUTCONTROLLER_H_
#define _CORE_CANVAS_ANIMATION_CREATOR_INPUTCONTROLLER_H_

#include "thorvg.h"
#include "core/input/input.h"
#include "editMode/editMode.h"

namespace core
{

class Scene;
class AnimationCreatorCanvas;
class InputController;

class AnimationCreatorInputController
{
public:
	using ThisClass = AnimationCreatorInputController;
	AnimationCreatorInputController(AnimationCreatorCanvas* canvas);
	void setMode(EditModeType mode);
	EditModeType getMode()
	{
		return mMode;
	}
	void onUpdate();

	bool onDoubleClickLefttMouse(const InputValue& inputValue);
	bool onStarClickLefttMouse(const InputValue& inputValue);
	bool onDragLeftMouse(const InputValue& inputValue);
	bool onMoveMouse(const InputValue& inputValue);
	bool onEndLeftMouse(const InputValue& inputValue);
	bool onInputDetach(const InputValue& inputValue);
	bool onInputAttach(const InputValue& inputValue);

	std::unique_ptr<InputController> mHandle;

private:
	void applyEditMode();

private:
	std::unique_ptr<EditMode> mEditMode;
	EditModeType mMode;
	AnimationCreatorCanvas* rCanvas;
};

}	 // namespace core

#endif