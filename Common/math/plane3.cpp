
#include "stdafx.h"
#include "plane3.h"

#ifdef USE_D3D9_MATH
	#include <D3dx9math.h>
#endif 

using namespace common;

Plane3::Plane3()
{
}

// initialize plane
// axis1, axis2 identity vector
// axis1, axis2 orthogonal
// axis1, axis2 clockwise
Plane3::Plane3(const Vector3& axis1, const Vector3& axis2, const Vector3& pos
	, const float size)
{
	N = axis1.CrossProduct(axis2).Normal();
	D = -N.DotProduct(pos);

	const Vector3 v0 = axis1 * size + pos;
	const Vector3 v1 = axis2 * size + pos;
	const Vector3 v2 = axis1 * -size + pos;
	const Vector3 v3 = axis2 * -size + pos;

	//         /.\ v0
	//          |
	//          |
	//          |
	//  v3<-----+----->v1
	//          |
	//          |
	//          |
	//         \./ v2
	tri[0] = Triangle(pos, v0, v1);
	tri[1] = Triangle(pos, v1, v2);
	tri[2] = Triangle(pos, v2, v3);
	tri[3] = Triangle(pos, v3, v0);
}


float Plane3::Distance(const Vector3& vP) const
{
	float fDot = N.DotProduct(vP);
	return fDot + (D);
}


Vector3 Plane3::Pick(const Vector3& vOrig, const Vector3& vDir) const
{
	for (int i = 0; i < 4; ++i)
	{
		float t, u, v;
		if (tri[i].Intersect(vOrig, vDir, &t, &u, &v))
		{
			Vector3 p0, p1;
			common::lerp(p0, tri[i].a, tri[i].b, u);
			common::lerp(p1, tri[i].a, tri[i].c, v);
			return p0 + p1 - tri[i].a;
		}
	}
	return tri[0].a;
}
