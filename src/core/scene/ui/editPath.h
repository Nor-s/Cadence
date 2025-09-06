#ifndef _CORE_SCENE_UI_EDITPATH_H_
#define _CORE_SCENE_UI_EDITPATH_H_

#include "scene/entity.h"
#include "scene/component/components.h"

#include <array>

namespace core
{

class InputValue;
class UIShape;
class Scene;

class EditPath
{
public:
	EditPath(core::Scene* scene, Entity target, bool isAddMode);
	~EditPath();

	void init();

	void onUpdate();
	bool onStartClickLeftMouse(const InputValue& inputValue);
	bool onDragLeftMouse(const InputValue& inputValue);
	bool onEndLeftMouse(const InputValue& inputValue);
	bool onMoveMouse(const InputValue& inputValue);

private:
	void initPreview();
	void initPath();
	void initControlPoint();

	void addPathPointControl(const Vec2& worldPosition);
	void addPathPoint(const Vec2& worldPosition);

	bool onStartClickControlForCurvePoint();
	bool onStartClickForPathPoint();

	void updateControlPoint();
	void updatePreview(const Vec2& endPoint);

private:
	Entity rTarget;
	Scene* rScene;
	bool mIsAddMode;
	PathPoints* rPathPoints;

	// mouse info
	Vec2 mStartPoint{0.0f, 0.0f};
	Vec2 mBeforePoint{0.0f, 0.0f};
	Vec2 mCurrentPoint{0.0f, 0.0f};

	// state
	PathPoint::Command mCurrentEditType{PathPoint::Command::LineTo};
	int mCurrentPointIndex = 0;
	UIShape* rCurrentUi{nullptr};
	bool mIsDrag{false};
	bool mIsAddPoint{false};

	// path
	std::vector<std::unique_ptr<UIShape>> mPathPointUIs;
	std::unique_ptr<UIShape> mControlLineUI;
	std::unique_ptr<UIShape> mLeftControlUI;
	std::unique_ptr<UIShape> mRightControlUI;
	std::unique_ptr<UIShape> mPathPreviewUI;
};

}	 // namespace core

#endif