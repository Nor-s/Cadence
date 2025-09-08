#ifndef _EDITOR_IMGUI_IMGUICANVAS_H_
#define _EDITOR_IMGUI_IMGUICANVAS_H_

#include <string_view>

namespace core
{
class CanvasWrapper;
class Entity;
}	 // namespace core

namespace editor
{

class ImGuiCanvasView
{
public:
	static core::CanvasWrapper* gCurrentCanvas;
	void onDraw(std::string_view title, core::CanvasWrapper& canvas, int canvasIndex);
	void onDrawSceneInspect();
	void onDrawContentBrowser();

private:
	void drawExampleCanvasContent();
	void drawAnimationCanvasProperties();

	void drawComponent(core::Entity& entity);
	void drawRectComponent(core::Entity& entity);
	void drawElipseComponent(core::Entity& entity);
	void drawPolygonComponent(core::Entity& entity);
	void drawStarPolygonComponent(core::Entity& entity);
};

}	 // namespace editor

#endif