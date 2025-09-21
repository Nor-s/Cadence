#include "animationCreatorInputController.h"

#include "input/input.h"
#include "input/inputAction.h"
#include "input/inputController.h"

#include "canvas.h"
#include "animationCreatorCanvas.h"
#include "animation/animator.h"

#include "common/common.h"

#include "editMode/pickMode.h"
#include "editMode/addMode.h"
#include "editMode/addPathMode.h"

#include "interface/editInterface.h"
#include "selection/selectionManager.h"

namespace core
{

AnimationCreatorInputController::AnimationCreatorInputController(AnimationCreatorCanvas* canvas)
{
	rCanvas = canvas;

	mHandle = std::make_unique<InputController>();

	mHandle->bindAction(InputAction(InputType::MOUSE_LEFT_DOUBLE_CLICK), InputTrigger::Triggered, this,
						&ThisClass::onDoubleClickLefttMouse);
	mHandle->bindAction(InputAction(InputType::MOUSE_LEFT_DOWN), InputTrigger::Started, this,
						&ThisClass::onStarClickLefttMouse);
	mHandle->bindAction(InputAction(InputType::MOUSE_LEFT_DOWN), InputTrigger::Triggered, this,
						&ThisClass::onDragLeftMouse);
	mHandle->bindAction(InputAction(InputType::MOUSE_LEFT_DOWN), InputTrigger::Ended, this, &ThisClass::onEndLeftMouse);
	mHandle->bindAction(InputAction(InputType::MOUSE_MOVE), InputTrigger::Triggered, this, &ThisClass::onMoveMouse);
	mHandle->bindAction(InputAction(InputType::INPUT_DETACH), InputTrigger::Triggered, this, &ThisClass::onInputDetach);
	mHandle->bindAction(InputAction(InputType::INPUT_ATTACH), InputTrigger::Triggered, this, &ThisClass::onInputAttach);
	mHandle->bindAction(InputAction(InputType::MOUSE_WHEEL), InputTrigger::Triggered, this, &ThisClass::onInputWheel);

	mHandle->bindAction(InputAction(InputType::MOUSE_MIDDLE_DOWN), InputTrigger::Started, this,
						&ThisClass::onStartMoveCanvas);
	mHandle->bindAction(InputAction(InputType::MOUSE_MIDDLE_DOWN), InputTrigger::Triggered, this,
						&ThisClass::onMoveCanvas);
	mHandle->bindAction(InputAction(InputType::MOUSE_MIDDLE_DOWN), InputTrigger::Ended, this,
						&ThisClass::onEndMoveCanvas);
	setMode(EditModeType::PICK);
}

void AnimationCreatorInputController::onUpdate()
{
	if (mEditMode)
	{
		mEditMode->onUpdate();
	}
}

void AnimationCreatorInputController::setMode(EditModeType mode)
{
	LOG_INFO("change edit mode: {}", (int) mMode);
	mMode = mode;
	applyEditMode();
}

bool AnimationCreatorInputController::onDoubleClickLefttMouse(const InputValue& inputValue)
{
	if (mEditMode == nullptr)
		return false;
	return mEditMode->onDoubleClickLeftMouse(inputValue);
}

bool AnimationCreatorInputController::onStarClickLefttMouse(const InputValue& inputValue)
{
	LOG_INFO("start edit: mode is {}", (int) mMode);
	rCanvas->mAnimator->stop();

	if (mEditMode == nullptr)
		return false;

	return mEditMode->onStarClickLefttMouse(inputValue);
}

bool AnimationCreatorInputController::onDragLeftMouse(const InputValue& inputValue)
{
	if (mEditMode == nullptr)
		return false;

	return mEditMode->onDragLeftMouse(inputValue);
}
bool AnimationCreatorInputController::onMoveMouse(const InputValue& inputValue)
{
	if (mEditMode == nullptr)
		return false;

	return mEditMode->onMoveMouse(inputValue);
}

bool AnimationCreatorInputController::onEndLeftMouse(const InputValue& inputValue)
{
	LOG_INFO("end edit: mode is {}", (int) mMode);
	if (mEditMode == nullptr)
		return false;

	if (mEditMode->onEndLeftMouse(inputValue))
	{
		switch (mMode)
		{
			case EditModeType::ADD_ELLIPSE:
			case EditModeType::ADD_POLYGON:
			case EditModeType::ADD_STAR:
			case EditModeType::ADD_SQUARE:
			case EditModeType::ADD_PEN_PATH:
			{
				setMode(EditModeType::PICK);
				break;
			}
		}
		return true;
	}
	return false;
}
bool AnimationCreatorInputController::onInputDetach(const InputValue& inputValue)
{
	LOG_INFO("detach input");

	if (mEditMode == nullptr)
		return false;

	return mEditMode->onInputDetach(inputValue);
}
bool AnimationCreatorInputController::onInputAttach(const InputValue& inputValue)
{
	if (mEditMode)
		return mEditMode->onInputAttach(inputValue);
	else
		applyEditMode();

	return true;
}

bool AnimationCreatorInputController::onInputWheel(const InputValue& inputValue)
{
	auto v = inputValue.get<Vec2>();
	rCanvas->mCanvasScene->mSceneEntity.setScaleByDelta(Vec2{v.y, v.y} * 0.01f);
	return false;
}

bool AnimationCreatorInputController::onStartMoveCanvas(const InputValue& inputValue)
{
	return false;
}

bool AnimationCreatorInputController::onMoveCanvas(const InputValue& inputValue)
{
	auto delta = inputValue.getDelta<Vec2>();
	rCanvas->mCanvasScene->mSceneEntity.moveByDelta(delta);

	return false;
}

bool AnimationCreatorInputController::onEndMoveCanvas(const InputValue& inputValue)
{
	return false;
}

// todo: no reset unique_ptr
void AnimationCreatorInputController::applyEditMode()
{
	RemoveSelection();
	switch (mMode)
	{
		case EditModeType::NONE:
		{
			mEditMode = nullptr;
			break;
		}
		case EditModeType::PICK:
		{
			mEditMode = std::make_unique<PickMode>(rCanvas);
			break;
		}
		case EditModeType::ADD_SQUARE:
		case EditModeType::ADD_ELLIPSE:
		case EditModeType::ADD_POLYGON:
		case EditModeType::ADD_STAR:
		{
			mEditMode = std::make_unique<AddMode>(rCanvas, mMode);
			break;
		}
		case EditModeType::ADD_PEN_PATH:
		{
			mEditMode = std::make_unique<AddPathMode>(rCanvas);
			break;
		}
		case EditModeType::EDIT_PEN_PATH:
		{
			mEditMode = nullptr;
			break;
		}
	};
}

}	 // namespace core