#include "path.h"
#include "components.h"

namespace core
{

void RectPath::appendTo(ShapeComponent& s)
{
	Update(s, *this);
}

void EllipsePath::appendTo(ShapeComponent& s)
{
	Update(s, *this);
}

void RawPath::appendTo(ShapeComponent& s)
{
	Update(s, *this);
}

void PolygonPath::appendTo(ShapeComponent& s)
{
	Update(s, *this);
}

void StarPolygonPath::appendTo(ShapeComponent& s)
{
	Update(s, *this);
}

IPath::IPath()
{
}

IPath::~IPath()
{
}

PathListComponent::PathListComponent()
{
}

PathListComponent::~PathListComponent()
{
}

}	 // namespace core
