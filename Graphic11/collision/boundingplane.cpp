
#include "stdafx.h"
#include "boundingplane.h"


using namespace graphic;

cBoundingPlane::cBoundingPlane()
{
	m_type = eCollisionType::PLANE;
}

cBoundingPlane::cBoundingPlane(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
	m_type = eCollisionType::PLANE;
	SetVertex(v0, v1, v2, v3);
}

cBoundingPlane::~cBoundingPlane()
{
}


// SetVertex
// Vertex Order = v0,v1,v2,v3 
//  0 ----------- 1
//  | \           |
//  |   \         |
//  |      +      |
//  |        \    |
//  |          \  |
//  3-------------2
//
void cBoundingPlane::SetVertex(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
	m_vertices[0] = v0;
	m_vertices[1] = v1;
	m_vertices[2] = v2;
	m_vertices[3] = v3;
}


bool cBoundingPlane::Collision(const cCollisionObj &obj
	, OUT Vector3 *outPos //= NULL
	, OUT float *distance //= NULL
) const
{
	switch (obj.m_type)
	{
	case eCollisionType::SPHERE:
	{
		const cBoundingSphere *p = dynamic_cast<const cBoundingSphere*>(&obj);
		if (!p)
			break;

		const Vector3 &pos = *(Vector3*)&p->m_bsphere.Center;
		const Plane plane(m_vertices[0], m_vertices[1], m_vertices[2]);
		const float d0 = plane.Collision(pos);
		if (abs(d0) > p->m_bsphere.Radius) // distance test
			break;

		const Triangle tri1(m_vertices[0], m_vertices[1], m_vertices[2]);
		const Triangle tri2(m_vertices[0], m_vertices[2], m_vertices[3]);
		const Vector3 norm = tri1.Normal();

		float u1, v1, t1 = FLT_MAX;
		float u2, v2, t2 = FLT_MAX;
		tri1.Intersect(pos, -norm, &t1, &u1, &v1);
		tri2.Intersect(pos, -norm, &t2, &u2, &v2);

		if (min(t1, t2) <= p->m_bsphere.Radius)
		{
			if (t1 < t2)
			{
				if (distance)
					*distance = t1;
				if (outPos)
					*outPos = tri1.a.Interpolate(tri1.b, u1)
						+ tri1.a.Interpolate(tri1.c, v1)
						- tri1.a;
			}
			else
			{
				if (distance)
					*distance = t2;
				if (outPos)
					*outPos = tri2.a.Interpolate(tri2.b, u2)
						+ tri2.a.Interpolate(tri2.c, v2)
						- tri2.a;
			}

			return true;
		}
	}
	break;

	default:
		assert(0);
		break;
	}

	return false;
}


bool cBoundingPlane::Pick(const Ray &ray
	, OUT float *distance //= NULL
) const
{
	Plane plane(m_vertices[0], m_vertices[1], m_vertices[2]);
	const float d0 = plane.Collision(ray.orig);
	if (d0 < 0) // back plane test
		return false;

	{
		float u, v, t;
		Triangle tri(m_vertices[0], m_vertices[1], m_vertices[2]);
		if (tri.Intersect(ray.orig, ray.dir, &t, &u, &v))
		{
			if (distance)
				*distance = t;
			return true;
		}
	}
	
	{
		float u, v, t;
		Triangle tri(m_vertices[0], m_vertices[2], m_vertices[3]);
		if (tri.Intersect(ray.orig, ray.dir, &t, &u, &v))
		{
			if (distance)
				*distance = t;
			return true;
		}
	}

	return false;
}


Transform cBoundingPlane::GetTransform() const
{
	Transform tfm;
	tfm.pos = (m_vertices[0] + m_vertices[1] + m_vertices[2] + m_vertices[3]) / 4.f;

	return tfm;
}


Vector3 cBoundingPlane::Normal() const
{
	Triangle tri1(m_vertices[0], m_vertices[1], m_vertices[2]);
	return tri1.Normal();
}
