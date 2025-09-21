#ifndef _CORE_INTERFACE_EDIT_HELPER_H_
#define _CORE_INTERFACE_EDIT_HELPER_H_

#include "editInterface.h"
#include "../scene/entity.h"

namespace core
{
class Scene;
class PathPoint;
}	 // namespace core

core::Scene* FindScene(SCENE_ID id);
int CurrentFrameNo();
bool IsPlaybackStopped();
core::Entity GetEntity(ENTITY_ID id);
bool FetchPath(core::Entity e, int idx, core::IPath*& out);
bool ShouldAddKeyframe();

template <class PathT, class ValueT, class MemberT, class KFMemberT>
Edit_Result SetProp(ENTITY_ID id,
					int pathIndex,
					const ValueT& value,
					MemberT PathT::*member,
					KFMemberT PathT::*kfMember,
					int isEnd)
{
	auto e = GetEntity(id);
	if (e.isNull())
		return EDIT_RESULT_INVALID_ENTITY;

	core::IPath* base = nullptr;
	if (!FetchPath(e, pathIndex, base))
		return EDIT_RESULT_INVALID_INDEX;

	if (base->type() != core::PathTag<PathT>::type)
		return EDIT_RESULT_TYPE_MISMATCH;

	auto* p = static_cast<PathT*>(base);
	p->*member = value;

	if (ShouldAddKeyframe())
	{
		(p->*kfMember).add(CurrentFrameNo(), p->*member);
	}

	e.setDirty(core::Dirty::Type::Path);

	return EDIT_RESULT_SUCCESS;
}

Edit_PathPoint ToEdit_PathPoint(const core::PathPoint& p);

#endif
