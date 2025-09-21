#ifndef _CORE_SCENE_COMPONENT_UI_COMPONENTS_H_
#define _CORE_SCENE_COMPONENT_UI_COMPONENTS_H_

#include <memory>
#include "scene/ui/bbox.h"
#include "scene/ui/editPath.h"

namespace core
{

struct BBoxControlComponent
{
	std::unique_ptr<BBox> bbox{nullptr};
};

struct EditPathControlComponent
{
	std::unique_ptr<EditPath> editPath{nullptr};
};

}	 // namespace core

#endif