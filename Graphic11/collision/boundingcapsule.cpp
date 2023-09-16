
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
			return Intersects(*p, distance);
		break;
	case eCollisionType::CAPSULE:
		if (const cBoundingCapsule* p = dynamic_cast<const cBoundingCapsule*>(&obj))
			return Intersects(*p, distance);
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
// tricky code: convert capsule to two sphere, one box and then check collision
// outGap: return intersect distance
bool cBoundingCapsule::Intersects(const cBoundingBox& bbox
	, float* outGap //= nullptr
) const
{
	// generate two sphere
	cBoundingSphere sp0((m_line.dir * m_line.len) + m_line.pos, m_radius);
	cBoundingSphere sp1((m_line.dir * -m_line.len) + m_line.pos, m_radius);
	// generate one box
	cBoundingBox box0;
	box0.SetLineBoundingBox(sp0.GetPos(), sp1.GetPos(), m_radius);
	return bbox.Collision(sp0) || bbox.Collision(sp1) || bbox.Collision(box0);
}


// warnning!!, this code contain bug!!
// intersect test with bounding box
// outGap: return intersect distance
// reference: 
//	- https://gamedev.stackexchange.com/questions/166450/get-closest-point-on-box-to-line
//	- https://math.stackexchange.com/questions/2213165/find-shortest-distance-between-lines-in-3d
bool cBoundingCapsule::Intersects2(const cBoundingBox& bbox
	, float* outGap //= nullptr
) const
{
	float dist = FLT_MAX;
	if (bbox.Pick(m_line.pos, m_line.dir, &dist))
		if (dist < (m_line.len + m_radius))
			return true; // intersect

	// 8 vertex, 12 edge
	// 
	//   Y axis
	//   |
	//   |   / Z axis
	//   |  /
	//   | /
	//   --------------> X axis
	// 
	//     1 - - - - 2
	//    /|        /|
	//   / |       / |
	//  0 - - - - 3  |
	//  |  |      |  |
	//  |  5 - - -|- 6
	//  | /       | /
	//  |/        |/
	//  4 - - - - 7
	//
	Vector3 vertices[8] = {
		//Vector3(-1, 1, -1), Vector3(-1, 1, 1), Vector3(1, 1, 1), Vector3(1, 1, -1),
		//Vector3(-1, -1, -1), Vector3(-1, -1, 1), Vector3(1, -1, 1), Vector3(1, -1, -1),

		// tricky code: center edge
		Vector3(-1, 0.1f, -1), Vector3(-1, 0.1f, 1), Vector3(1, 0.1f, 1), Vector3(1, 0.1f, -1),
		Vector3(-1, -0.1f, -1), Vector3(-1, -0.1f, 1), Vector3(1, -0.1f, 1), Vector3(1, -0.1f, -1),
	};
	const Matrix44 mat = bbox.GetMatrix();
	for (int i = 0; i < 8; ++i)
		vertices[i] = vertices[i] * mat;
	const int edges[12][2] = {
		{0, 1}, {0, 3}, {0, 4},
		{1, 2}, {1, 5},
		{2, 3}, {2, 6},
		{3, 7},
		{4, 5}, {4, 7},
		{5, 6},
		{6, 7},
	};
	for (int i = 0; i < 12; ++i)
	{
		const Vector3& p0 = vertices[edges[i][0]];
		const Vector3& p1 = vertices[edges[i][1]];
		const Line line(p0, p1);
		const float d = m_line.GetDistance(line);
		if (d < m_radius)
			return true; // intersect!!
	}
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


// update capsule
void cBoundingCapsule::SetCapsule(const cBoundingBox& bbox)
{
	// find most distance axis
	float len1, len2;
	Vector3 axis;
	if ((bbox.m_bbox.Extents.x >= bbox.m_bbox.Extents.y)
		&& (bbox.m_bbox.Extents.x >= bbox.m_bbox.Extents.z))
	{
		len1 = bbox.m_bbox.Extents.x;
		len2 = max(bbox.m_bbox.Extents.y, bbox.m_bbox.Extents.z);
		axis = Vector3(1, 0, 0);
	}
	else if (bbox.m_bbox.Extents.y >= bbox.m_bbox.Extents.z)
	{
		len1 = bbox.m_bbox.Extents.y;
		len2 = max(bbox.m_bbox.Extents.x, bbox.m_bbox.Extents.z);
		axis = Vector3(0, 1, 0);
	}
	else
	{
		len1 = bbox.m_bbox.Extents.z;
		len2 = max(bbox.m_bbox.Extents.x, bbox.m_bbox.Extents.y);
		axis = Vector3(0, 0, 1);
	}

	const Vector3 dir = axis * bbox.GetRotation();
	SetCapsule(bbox.Center(), dir, len1, len2);
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
