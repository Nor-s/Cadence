#ifndef _CORE_CANVAS_EDITMODE_ADD_PATH_MODE_H_
#define _CORE_CANVAS_EDITMODE_ADD_PATH_MODE_H_

#include "editMode.h"
#include "scene/entity.h"
#include "scene/component/components.h"
#include "../shapeUtil.h"

namespace core
{
class Scene;
class ControlOverlay;

class AddPathMode : public EditMode
{
	struct Context
	{
		Vec2 startPos;
		Vec2 beforePos;
		Vec2 currentPos;
		PathPoints path;
		std::vector<std::unique_ptr<ControlOverlay>> controlList;
		int clickedIndex{-1};
		PathPoint::Command currentEditType{PathPoint::Command::LineTo};
		Entity line;

		std::unique_ptr<ControlOverlay> preview;
		std::unique_ptr<ControlOverlay> controlLine;
		std::unique_ptr<ControlOverlay> leftControl;
		std::unique_ptr<ControlOverlay> rightControl;
		int controlIndex{-1};

		ControlOverlay* pickedControl = nullptr;
		bool activePreview = true;

		Context();
		~Context();
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

	void genCurrentPointControl();
	void updateCurrentPointControl();

private:
	AnimationCreatorCanvas* rCanvas{nullptr};
	Context mContext{};
};
}	 // namespace core

#endif