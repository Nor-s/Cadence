#ifndef _EDITOR_IMGUI_IMGUICANVAS_H_
#define _EDITOR_IMGUI_IMGUICANVAS_H_

#include <string_view>

namespace core
{
class CanvasWrapper;
class Entity;
struct RawPath;
struct RectPath;
struct EllipsePath;
struct PolygonPath;
struct StarPolygonPath;
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
	void drawRectComponent(core::Entity& entity, core::RectPath& path, int idx);
	void drawEllipseComponent(core::Entity& entity, core::EllipsePath& path, int idx);
	void drawPolygonComponent(core::Entity& entity, core::PolygonPath& path, int idx);
	void drawStarPolygonComponent(core::Entity& entity, core::StarPolygonPath& path, int idx);
	void drawPathComponent(core::Entity& entity, core::RawPath& path, int idx);
};

}	 // namespace editor

#endif