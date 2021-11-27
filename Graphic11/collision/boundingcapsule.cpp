
#include "stdafx.h"
#include "boundingcapsule.h"

using namespace common;


cBoundingCapsule::cBoundingCapsule(
	const float radius //= 1.0f
	, const float halfHeight //= 1.0f
)
	: m_radius(radius)
{
	m_line.len = halfHeight;
}

cBoundingCapsule::~cBoundingCapsule()
{
}


// collision
bool cBoundingCapsule::Collision(const cCollisionObj &obj
	, OUT Vector3 *outPos //= NULL
	, OUT float *distance //= NULL
) const
{
	// nothing~
	return false;
}


// picking
bool cBoundingCapsule::Pick(const Ray &ray
	, OUT float *distance //= NULL
) const
{
	// nothing~
	return false;
}


// get transform
Transform cBoundingCapsule::GetTransform() const
{
	Transform tfm;
	return tfm;
}


// intersect test with bounding capusle
// outGap: return intersect distance
bool cBoundingCapsule::Intersects(const cBoundingCapsule &bcapsule
	, float *outGap //= nullptr
) const
{
	const float dist = m_line.GetDistance(bcapsule.m_line);
	if (outGap)
		*outGap = abs(dist - (m_radius + bcapsule.m_radius));
	return dist <= (m_radius + bcapsule.m_radius);
}


// intersect test with bounding sphere
// outGap: return intersect distance
bool cBoundingCapsule::Intersects(const cBoundingSphere &bsphere
	, float *outGap //= nullptr
) const
{
	const float dist = m_line.GetDistance(bsphere.GetPos());
	if (outGap)
		*outGap = abs(dist - (m_radius + bsphere.GetRadius()));
	return dist <= (m_radius + bsphere.GetRadius());
}


// set bounding capsule
// pos1, pos2: capsule start, end point pos
// radius: capsule head tail radius
// total height: distance(pos1, pos2) + (radius x 2)
void cBoundingCapsule::SetCapsule(const Vector3 &pos0, const Vector3 &pos1
	, const float radius)
{
	m_line = Line(pos0, pos1);
	m_radius = radius;
}


void cBoundingCapsule::SetPos(const Vector3 &pos) 
{
	m_line.pos = pos;
}


const Vector3& cBoundingCapsule::GetPos() const
{
	return m_line.pos;
}
