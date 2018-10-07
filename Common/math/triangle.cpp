
#include "stdafx.h"
#include "triangle.h"


using namespace common;

Triangle::Triangle()
{

}

Triangle::Triangle( const Vector3& v1, const Vector3& v2, const Vector3 &v3 ):	
a(v1), b(v2), c( v3 )
{
}

// Init
void Triangle::Create( const Vector3& v1, const Vector3& v2, const Vector3& v3 )
{
	a = v1;
	b = v2;
	c = v3;
}


// Triangle::Intersect
// pfT, pfU, pfV : out
// pfT : distance
// collision pos = lerp(a, b, pfU) + lerp(a, c, pfV) - a
bool Triangle::Intersect( const Vector3& vOrig, const Vector3& vDir, float *pfT, float *pfU, float *pfV	)  const
{
	static float u, v;

	// Find vectors for two edges sharing vert0

	static Vector3	vEdge1;
	static Vector3	vEdge2;

	vEdge1 = b - a;
	vEdge2 = c - a;

	// Begin calculating determinant - also used to calculate U parameter

	static Vector3	vP;
	vP = vDir.CrossProduct( vEdge2 );

	// If determinant is near zero, ray lies in plane of triangle

	float fDet = vEdge1.DotProduct( vP );
	if( fDet < 0.0001F )
	{
		return false;
	} //if

	// Calculate distance from vert0 to ray origin

	static Vector3	vT;
	vT = vOrig - a;

	// Calculate U parameter and test bounds

	u = vT.DotProduct( vP );
	if( u < 0.0F || u > fDet )
	{
		return false;
	} //if

	// Prepare to test V parameter

	static Vector3 vQ;
	vQ = vT.CrossProduct( vEdge1 );

	// Calculate V parameter and test bounds

	v = vDir.DotProduct( vQ );

	if( v < 0.0F || u + v > fDet )
	{
		return false;
	} //if

	// Calculate t, scale parameters, ray intersects triangle

	float fInvDet = 1.0F / fDet;

	const float _t = vEdge2.DotProduct(vQ) * fInvDet;
	const float _u = u * fInvDet;
	const float _v = v * fInvDet;

	if (pfT) *pfT = _t;
	if (pfU) *pfU = _u;
	if (pfV) *pfV = _v;

	// intersect back of triangle return false
	if (_t < -0.001f)
		return false;

	return true;
}


// vPos와 Triangle 중점의 거리를 리턴한다.
float Triangle::Distance( const Vector3& vPos )  const
{
	Vector3 center;
	center.x = (a.x + b.x + c.x) / 3.f;
	center.y = (a.y + b.y + c.y) / 3.f;
	center.z = (a.z + b.z + c.z) / 3.f;
	return (vPos - center).Length();
}


Vector3 Triangle::Normal() const
{
	const Vector3 v0 = (b - a).Normal();
	const Vector3 v1 = (c - a).Normal();
	return v0.CrossProduct(v1).Normal();
}


Vector3 Triangle::GetClosesPointOnTriangle(const Vector3 &sourcePosition) const
{
	const Vector3 triangle[3] = { a, b, c };

	Vector3 edge0 = triangle[1] - triangle[0];
	Vector3 edge1 = triangle[2] - triangle[0];
	Vector3 v0 = triangle[0] - sourcePosition;

	float a = edge0.DotProduct(edge0);
	float b = edge0.DotProduct(edge1);
	float c = edge1.DotProduct(edge1);
	float d = edge0.DotProduct(v0);
	float e = edge1.DotProduct(v0);

	float det = a * c - b * b;
	float s = b * e - c * d;
	float t = b * d - a * e;

	if (s + t < det)
	{
		if (s < 0.f)
		{
			if (t < 0.f)
			{
				if (d < 0.f)
				{
					s = clamp2(-d / a, 0.f, 1.f);
					t = 0.f;
				}
				else
				{
					s = 0.f;
					t = clamp2(-e / c, 0.f, 1.f);
				}
			}
			else
			{
				s = 0.f;
				t = clamp2(-e / c, 0.f, 1.f);
			}
		}
		else if (t < 0.f)
		{
			s = clamp2(-d / a, 0.f, 1.f);
			t = 0.f;
		}
		else
		{
			float invDet = 1.f / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else
	{
		if (s < 0.f)
		{
			float tmp0 = b + d;
			float tmp1 = c + e;
			if (tmp1 > tmp0)
			{
				float numer = tmp1 - tmp0;
				float denom = a - 2 * b + c;
				s = clamp2(numer / denom, 0.f, 1.f);
				t = 1 - s;
			}
			else
			{
				t = clamp2(-e / c, 0.f, 1.f);
				s = 0.f;
			}
		}
		else if (t < 0.f)
		{
			if (a + d > b + e)
			{
				float numer = c + e - b - d;
				float denom = a - 2 * b + c;
				s = clamp2(numer / denom, 0.f, 1.f);
				t = 1 - s;
			}
			else
			{
				s = clamp2(-e / c, 0.f, 1.f);
				t = 0.f;
			}
		}
		else
		{
			float numer = c + e - b - d;
			float denom = a - 2 * b + c;
			s = clamp2(numer / denom, 0.f, 1.f);
			t = 1.f - s;
		}
	}

	return triangle[0] + edge0 * s + edge1 * t;
}
