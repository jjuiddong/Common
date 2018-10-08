
#include "stdafx.h"
#include "boundingplane.h"


using namespace common;

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
	m_plane = Plane(v0, v1, v2);
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
		const float d0 = m_plane.Collision(pos);
		if (abs(d0) > p->m_bsphere.Radius) // distance test
			break;

		const Triangle tri1(m_vertices[0], m_vertices[1], m_vertices[2]);
		const Triangle tri2(m_vertices[0], m_vertices[2], m_vertices[3]);
		//const Vector3 norm = tri1.Normal();
		//float u1, v1, t1 = FLT_MAX;
		//float u2, v2, t2 = FLT_MAX;
		//tri1.Intersect(pos, -norm, &t1, &u1, &v1);
		//tri2.Intersect(pos, -norm, &t2, &u2, &v2);
		const Vector3 closetPt1 = tri1.GetClosesPointOnTriangle(pos);
		const Vector3 closetPt2 = tri2.GetClosesPointOnTriangle(pos);

		const float len1 = closetPt1.Distance(pos);
		const float len2 = closetPt2.Distance(pos);
		if (min(len1, len2) <= p->m_bsphere.Radius)
		{
			if (len1 < len2)
			{
				if (distance)
					*distance = len1;
				if (outPos)
					*outPos = closetPt1;
			}
			else
			{
				if (distance)
					*distance = len2;
				if (outPos)
					*outPos = closetPt2;
			}

			return true;
		}

		//if (min(t1, t2) <= p->m_bsphere.Radius)
		//{
		//	if (t1 < t2)
		//	{
		//		if (distance)
		//			*distance = t1;
		//		if (outPos)
		//			*outPos = tri1.a.Interpolate(tri1.b, u1)
		//				+ tri1.a.Interpolate(tri1.c, v1)
		//				- tri1.a;
		//	}
		//	else
		//	{
		//		if (distance)
		//			*distance = t2;
		//		if (outPos)
		//			*outPos = tri2.a.Interpolate(tri2.b, u2)
		//				+ tri2.a.Interpolate(tri2.c, v2)
		//				- tri2.a;
		//	}

		//	return true;
		//}
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
	const float d0 = m_plane.Collision(ray.orig);
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


// Ray가 radius 반지름을 가진 원기둥이라고 가정하고,
// 충돌 계산을 한다. x축으로만 확장해서 검사한다.
bool cBoundingPlane::Intersect(const Ray &ray, const float radius
	, OUT float *distance //= NULL
	, const int recursiveCnt // = 0
) const
{
	const Vector3 &pos = ray.orig;
	const float d0 = m_plane.Collision(ray.orig);
	if (d0 < 0) // back plane test
		return false;

	const Vector3 right = Vector3(0, 1, 0).CrossProduct(ray.dir).Normal();
	const Ray rayAr[3] = {
		ray
		, Ray(ray.orig + (right * radius), ray.dir)
		, Ray(ray.orig - (right * radius), ray.dir)
	};
	float dist[3] = {
		FLT_MAX, FLT_MAX, FLT_MAX
	};

	const Triangle tri1(m_vertices[0], m_vertices[1], m_vertices[2]);
	const Triangle tri2(m_vertices[0], m_vertices[2], m_vertices[3]);
	float minDist = FLT_MAX;
	int idx = -1;

	for (int i = 0; i < ARRAYSIZE(rayAr); ++i)
	{
		float u, v, t;
		if (tri1.Intersect(rayAr[i].orig, rayAr[i].dir, &t, &u, &v))
		{
			dist[i] = t;
			if (t < minDist)
			{
				minDist = t;
				idx = i;
			}
		}

		if (tri2.Intersect(rayAr[i].orig, rayAr[i].dir, &t, &u, &v))
		{
			dist[i] = t;
			if (t < minDist)
			{
				minDist = t;
				idx = i;
			}
		}
	}

	if (idx < 0)
		return false;

//	if ((recursiveCnt < 5) && (idx != 0)) // side intersection
	if ((recursiveCnt < 5) && (abs(dist[idx]-dist[0]) > (radius * 0.1f))) // side intersection
	{
		// center와 side의 사잇값으로 다시한번 체크
		Intersect(Ray((rayAr[idx].orig + rayAr[0].orig) / 2.f, rayAr[idx].dir)
			, radius * 0.5f, distance, recursiveCnt+1);


		//// bounding plane tangent vector n
		//Vector3 tv = Vector3(0, 1, 0).CrossProduct(m_plane.N).Normal();
		//if (ray.dir.DotProduct(tv) > 0.f)
		//	tv = -tv;
		//const float angle = abs( acos(ray.dir.DotProduct(tv)) ); // radian
		//if ((angle > 0.1f) && (angle < (MATH_PI / 2.f))
		//{
		//	// 기울어진 plane일 경우, center와는 충돌되지 않고, side ray만 충돌 될 경우
		//	// 
		//}
		//else
		//{
		//	// Plane과의 각도가 0에 가깝거나, 90에 가까우면 거리를 보정하지 않는다. 
		//	if (distance)
		//		*distance = minDist;
		//}
	}
	else
	{
		if (distance)
			*distance = minDist;
	}

	return true;


	//const Vector3 closetPt1 = tri1.GetClosesPointOnTriangle(pos);
	//const Vector3 closetPt2 = tri2.GetClosesPointOnTriangle(pos);
	//const float len1 = closetPt1.Distance(pos);
	//const float len2 = closetPt2.Distance(pos);
	////const float closetLen1 = common::GetShortestLen(ray.orig, ray.orig + ray.dir * len1 * 2.f, closetPt1);
	////const float closetLen2 = common::GetShortestLen(ray.orig, ray.orig + ray.dir * len2 * 2.f, closetPt2);
	//const float closetLen1 = common::GetShortestLen(ray, closetPt1);
	//const float closetLen2 = common::GetShortestLen(ray, closetPt2);

	//if (min(closetLen1, closetLen2) <= radius)
	//{
	//	if (closetLen1 < closetLen2)
	//	{
	//		if (distance)
	//			*distance = len1;
	//	}
	//	else
	//	{
	//		if (distance)
	//			*distance = len2;
	//	}

	//	return true;
	//}

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
