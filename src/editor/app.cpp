#include "app.h"
#include "window/glWindow.h"

#include "imgui/imguiManager.h"

#include <core/core.h>

#include "examples/examples.h"

#include "event/eventStack.h"
#include "event/events.h"

App& App::GetInstance()
{
	static App instance;
	return instance;
}

void App::InitInstance(const AppState& state)
{
	tvg::Initializer::init(0);
	GetInstance().mState = state;
	GetInstance().init();
}

void App::DestroyInstance()
{
	tvg::Initializer::term();
}

void App::MainLoop()
{
	App& app = GetInstance();
	while (app.mState.running)
	{
		app.loop();
	}
}

void App::init()
{
	mEventController = std::make_unique<editor::EventStack>();
	mWindow = std::make_unique<editor::GLWindow>(mState.resolution);
	mImguiManager = std::make_unique<editor::ImGuiManager>();

	// tvgGl.h
	glInit();
	extraGlInit();

	mCanvasList.push_back(new core::AnimationCreatorCanvas(mWindow->mContext, {500.0f, 500.0f}, false));
	float clearColor[3] = {.89f, 0.87f, 0.80f};
	for (auto& canvas : mCanvasList)
	{
		canvas->clearColor(clearColor);
		canvas->onInit();
	}
	focusCanvas(0);
	mWindow->show();
}

void App::loop()
{
	if (!processEvent())
	{
		return;
	}

	update();

	draw();
	drawgui();
	drawend();
}

void App::update()
{
	mWindow->update();
	for (auto& canvas : mCanvasList)
	{
		canvas->onUpdate();
	}
}

void App::draw()
{
	for (auto& canvas : mCanvasList)
	{
		canvas->draw();
	}
}

void App::drawgui()
{
	mImguiManager->draw();
	mImguiManager->drawCanvas(mCanvasList);
}

void App::drawend()
{
	mImguiManager->drawend();
	mWindow->drawend();
}
