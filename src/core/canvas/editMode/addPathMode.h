#ifndef _CORE_CANVAS_EDITMODE_ADD_PATH_MODE_H_
#define _CORE_CANVAS_EDITMODE_ADD_PATH_MODE_H_

#include "editMode.h"
#include "scene/entity.h"
#include "scene/component/components.h"
#include "../shapeUtil.h"

namespace core
{
class Scene;
class UIShape;
class EditPath;

class AddPathMode : public EditMode
{
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

	void init();

private:
	AnimationCreatorCanvas* rCanvas{nullptr};
	Entity mTargetPath;
	std::unique_ptr<EditPath> mEditPath{nullptr};
};
}	 // namespace core

#endif