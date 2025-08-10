#include "inputEventHandler.h"

using namespace core;

void InputEventHandler::setInputContoller(InputController* inputController)
{
    LOG_INFO("set input controller");
	if (rInputController && rInputController != inputController)
	{
		rInputController->broadcast(InputType::INPUT_DETACH, InputTrigger::Triggered, 1.0f);
		mState = State{};
	}
	rInputController = inputController;
    if(rInputController)
    {
	    rInputController->broadcast(InputType::INPUT_ATTACH, InputTrigger::Triggered, 1.0f);
    }
}

void InputEventHandler::processEvent(const SDL_Event& event)
{
    auto& io = ImGui::GetIO();
	if (rInputController == nullptr)
		return;

	const auto mouseOffset = core::io::mouseOffset;
	const auto mouseX = event.motion.x + mouseOffset.x;
	const auto mouseY = event.motion.y + mouseOffset.y;

	switch (event.type)
	{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			// todo
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				rInputController->broadcast(InputType::MOUSE_LEFT_DOWN, InputTrigger::Started, {mouseX, mouseY});
				mState.leftMouseDown = true;
			}
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			if (mState.leftMouseDown&&event.button.button == SDL_BUTTON_LEFT)
			{
				rInputController->broadcast(InputType::MOUSE_LEFT_DOWN, InputTrigger::Ended, {mouseX, mouseY});
				mState.leftMouseDown = false;
			}
			break;
		}
		case SDL_MOUSEMOTION:
		{
			mState.mousePos = {mouseX, mouseY};
			rInputController->broadcast(InputType::MOUSE_MOVE, InputTrigger::Triggered, InputValue(mouseX, mouseY));
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			const float dx = (event.wheel.preciseX != 0.0f) ? event.wheel.preciseX : (float) event.wheel.x;
			const float dy = (event.wheel.preciseY != 0.0f) ? event.wheel.preciseY : (float) event.wheel.y;
			rInputController->broadcast(InputType::MOUSE_WHEEL, InputTrigger::Triggered, InputValue(dx, dy));
			break;
		}
		default:
			break;
	}
}

void InputEventHandler::processTick()
{
	if (rInputController == nullptr)
		return;

	if (mState.leftMouseDown)
	{
		rInputController->broadcast(InputType::MOUSE_LEFT_DOWN, InputTrigger::Triggered, mState.mousePos);
	}
}