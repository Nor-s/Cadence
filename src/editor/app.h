#ifndef _EDITOR_APP_H_
#define _EDITOR_APP_H_

#include <core/core.h>

#include <string>
#include <memory>
#include "event/eventController.h"
#include "event/inputEventHandler.h"

namespace core
{
class CanvasWrapper;
class InputController;
}	 // namespace core
namespace editor
{
class Window;
class ImGuiManager;
}	 // namespace editor

class App
{
public:
	struct AppState
	{
		core::Size resolution;
		std::string title;
		bool running = true;
		AppState() : resolution(1280, 720), title("Cadence")
		{
		}
	};

	static App& GetInstance();
	static void InitInstance(const AppState& state = AppState());
	static void DestroyInstance();
	static void MainLoop();
	static const AppState& GetState()
	{
		return GetInstance().mState;
	}

	// event
	template <typename EventType, typename... Args>
	static void PushEvent(Args&&... args)
	{
		GetInstance().mEventController->push(std::make_unique<EventType>(std::forward<Args>(args)...));
	}
	static void CanvasResize(int canvasIndex, core::Size size);
	static void CavasFocus(int canvasIndex, bool isFocus);

public:
	App() = default;
	~App() = default;

	void init();
	void loop();
	void update();
	bool processEvent();
	void draw();
	void drawgui();
	void drawend();

	void focusCanvas(int canvasIndex);
	void setInputController(core::InputController* inputController);

private:
	AppState mState;
	std::unique_ptr<editor::ImGuiManager> mImguiManager;
	std::unique_ptr<editor::Window> mWindow;
	std::unique_ptr<editor::EventController> mEventController;
	std::vector<core::CanvasWrapper*> mCanvasList;

	int mCurrentFocusCanvas = 0;
	core::InputController* rInputController = nullptr;
	InputEventHandler mInputEventHandler;
};

#endif