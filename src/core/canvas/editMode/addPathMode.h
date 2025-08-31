#ifndef _CORE_CANVAS_EDITMODE_ADD_PATH_MODE_H_
#define _CORE_CANVAS_EDITMODE_ADD_PATH_MODE_H_

#include "editMode.h"
#include "scene/entity.h"
#include "scene/component/components.h"
#include "../shapeUtil.h"

namespace core
{
class Scene;
class ControlBox;

class AddPathMode : public EditMode
{
	struct Context
	{
		Vec2 startPos;
		Vec2 beforePos;
		Vec2 currentPos;
		PathList pathList;
		std::list<std::unique_ptr<ControlBox>> controlList;
		ControlBox* clickedControl{nullptr};
		PathPoint::Type currentEditType{PathPoint::Type::Line};
	};

public:
	AddPathMode(AnimationCreatorCanvas* canvas);
	~AddPathMode();
	void onUpdate() override;
	bool onStarClickLefttMouse(const InputValue& inputValue) override;
	bool onDragLeftMouse(const InputValue& inputValue) override;
	bool onMoveMouse(const InputValue& inputValue) override;
	bool onEndLeftMouse(const InputValue& inputValue) override;
	bool onInputAttach(const InputValue& inputValue) override;
	bool onInputDetach(const InputValue& inputValue) override;

private:
	AnimationCreatorCanvas* rCanvas{nullptr};
	Context mContext{};
};
}	 // namespace core

#endif