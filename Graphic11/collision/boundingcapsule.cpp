
#include "stdafx.h"
#include "boundingcapsule.h"

using namespace common;


cBoundingCapsule::cBoundingCapsule(
	const float radius //= 1.0f
	, const float halfHeight //= 1.0f
)
	: cCollisionObj(eCollisionType::CAPSULE)
	, m_radius(radius)
{
	m_line.len = halfHeight;
}

cBoundingCapsule::~cBoundingCapsule()
{
}


// collision
bool cBoundingCapsule::Collision(const cCollisionObj &obj
	, OUT Vector3 *outPos //= nullptr
	, OUT float *distance //= nullptr
	, OUT bool* isContain //= nullptr
) const
{
	switch (obj.m_type)
	{
	case eCollisionType::SPHERE:
		if (const cBoundingSphere* p = dynamic_cast<const cBoundingSphere*>(&obj))
			return Intersects(*p);
		break;
	case eCollisionType::CAPSULE:
		if (const cBoundingCapsule* p = dynamic_cast<const cBoundingCapsule*>(&obj))
			return Intersects(*p);
		break;
	case eCollisionType::BOX:
		if (const cBoundingBox* p = dynamic_cast<const cBoundingBox*>(&obj))
			return Intersects(*p);
		break;
	default:
		assert(0);
		break;
	}
	return false;
}


// picking
bool cBoundingCapsule::Pick(const Ray &ray
	, OUT float *distance //= NULL
) const
{
	// nothing~
	assert(0);
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


// intersect test with bounding box
// outGap: return intersect distance
bool cBoundingCapsule::Intersects(const cBoundingBox& bbox
	, float* outGap //= nullptr
) const
{
	assert(0);
	return false;
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


// set bounding capsule
// center: center pos
// dir: direction
// halfLen: capsule half length
void cBoundingCapsule::SetCapsule(const Vector3 &center, const Vector3 &dir
	, const float halfLen, const float radius)
{
	const Vector3 pos0 = center + dir * halfLen;
	const Vector3 pos1 = center + dir * -halfLen;
	m_line = Line(pos0, pos1);
	m_radius = radius;
}


// set bounding capsule with box size
// center: center pos
// dir: direction
// forwardLen: box forward length
// sideLen: box side length
void cBoundingCapsule::SetCapsuleBox(const Vector3& center, const Vector3& dir
	, const float forwardLen, const float sideLen)
{
	//const float halfLen = forwardLen - sideLen;
	//const float radius = sideLen * sqrt(2.f);
	const float halfLen = forwardLen;
	const float radius = sideLen;
	SetCapsule(center, dir, halfLen, radius);
}


void cBoundingCapsule::SetPos(const Vector3 &pos) 
{
	m_line.pos = pos;
}


const Vector3& cBoundingCapsule::GetPos() const
{
	return m_line.pos;
}
