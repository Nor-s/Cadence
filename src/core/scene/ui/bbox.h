#ifndef _CORE_CANVAS_EDITMODE_BBOX_H_
#define _CORE_CANVAS_EDITMODE_BBOX_H_

#include "scene/entity.h"
#include "scene/component/components.h"

#include <array>

namespace core
{

class InputActionBinding;
class InputController;
class InputValue;
class ControlOverlay;
class Scene;

class BBox
{
	enum ControlType
	{
		TopLeftScale = 0,
		TopRightScale,
		BottomLeftScale,
		BottomRightScale,
		TopCenterScale,
		BottomCenterScale,
		LeftCenterScale,
		RightCenterScale,

		BoxArea,
		AnchorPoint,

		TopLeftRotate,
		TopRightRotate,
		BottomLeftRotate,
		BottomRightRotate,

		ControlTypeCount
	};

public:
	BBox(InputController* inputController, core::Scene* scene, Entity target);
	~BBox();

	void retarget(Entity target);

	void onUpdate();
	bool onStartClickLeftMouse(const InputValue& inputValue);
	bool onDragLeftMouse(const InputValue& inputValue);
	bool onEndLeftMouse(const InputValue& inputValue);
	bool onMoveMouse(const InputValue& inputValue);

	Entity rTarget;

private:
	Scene* rScene{nullptr};
	TransformComponent mBeforeTransform;
	InputController* rInputController{nullptr};
	Vec2 mStartPoint{0.0f, 0.0f};
	Vec2 mBeforePoint{0.0f, 0.0f};
	Vec2 mCurrentPoint{0.0f, 0.0f};
	ControlType mCurrentControlType{ControlTypeCount};
	std::array<std::unique_ptr<ControlOverlay>, ControlTypeCount> mControlBox;
	std::vector<InputActionBinding*> mInputActionBindings;
	bool mIsDrag{false};
};

}	 // namespace core

#endif