#ifndef _EDITOR_EVENT_INPUT_EVENT_HANDLER_H_
#define _EDITOR_EVENT_INPUT_EVENT_HANDLER_H_

#include <core/core.h>

class InputEventHandler
{
	struct State
	{
		float clickTimer{0.0f};
		int clickCount{0};
		bool leftMouseDown{false};
		core::Vec2 mousePos{};
		void init()
		{
			leftMouseDown = false;
			clickCount = 0;
		}
	};
	inline static float Threshold_doubleClickTime = 0.1f;

public:
	void setInputContoller(core::InputController* inputController);
	void processEvent(const SDL_Event& event);
	void processTick();

private:
	State mState;
	core::InputController* rInputController = nullptr;
};

#endif