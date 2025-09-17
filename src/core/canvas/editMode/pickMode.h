#ifndef _CORE_CANVAS_EDITMODE_PICKMODE_H_
#define _CORE_CANVAS_EDITMODE_PICKMODE_H_

#include "editMode.h"
#include "scene/entity.h"
#include "../shapeUtil.h"

namespace core
{
class Scene;
class BBox;

class PickMode : public EditMode
{
	struct Context
	{
		// for outline
		PickInfo pickInfo{};
		Vec2 startPoint{};
		Vec2 beforePoint{};

		// Entity bbox;
		// Entity currentObb;
		// Entity drag;
		bool isLeftMouseDown{false};
	};

public:
	PickMode(AnimationCreatorCanvas* canvas);
	~PickMode();
	void onUpdate() override;
	bool pick(const InputValue& inputValue, tvg::Paint* paint, int depth);
	bool pick(const InputValue& inputValue);
	bool onDoubleClickLeftMouse(const InputValue& inputValue) override;
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