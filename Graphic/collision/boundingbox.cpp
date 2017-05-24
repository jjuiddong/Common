
#include "stdafx.h"
#include "boundingbox.h"

using namespace graphic;


cBoundingBox::cBoundingBox()
{
}

cBoundingBox::cBoundingBox(const cCube &cube)
{
	operator=(cube);
}


void cBoundingBox::SetBoundingBox(const Vector3 &vMin, const Vector3 &vMax )
{
	m_min = vMin;
	m_max = vMax;
}


void cBoundingBox::SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	Matrix44 matS;
	matS.SetScale(Vector3(width, width, len / 2.f));

	Matrix44 matT;
	matT.SetTranslate((p0 + p1) / 2.f);

	Quaternion q(Vector3(0, 0, 1), v);
	m_tm = matS * q.GetMatrix() * matT;
}


void cBoundingBox::SetTransform( const Matrix44 &tm )
{
	m_tm = tm;
}


void SATtest( const Vector3& axis, const Vector3 ptSet[8], float& minAlong, float& maxAlong )
{
	minAlong = (float)HUGE, maxAlong = (float)-HUGE;
	for( u_int i = 0 ; i < 8; i++ )
	{
		// just dot it to get the min/max along this axis.
		float dotVal = ptSet[ i].DotProduct( axis ) ;
		if( dotVal < minAlong )  minAlong=dotVal;
		if( dotVal > maxAlong )  maxAlong=dotVal;
	}
}


inline bool isBetweenOrdered( float val, float lowerBound, float upperBound ) {
	return lowerBound <= val && val <= upperBound ;
}

bool overlaps( float min1, float max1, float min2, float max2 )
{
	return isBetweenOrdered( min2, min1, max1 ) || isBetweenOrdered( min1, min2, max2 ) ;
}


// OBB vs OBB 충돌처리.
bool cBoundingBox::Collision( cBoundingBox &box )
{
	//vector<Vector3> normals(6);
	Vector3 normals[ 6];
	normals[ 0] = Vector3(0,1,0).MultiplyNormal(m_tm);
	normals[ 1] = Vector3(0,0,1).MultiplyNormal(m_tm);
	normals[ 2] = Vector3(1,0,0).MultiplyNormal(m_tm);
	normals[ 3] = Vector3(0,1,0).MultiplyNormal(box.m_tm);
	normals[ 4] = Vector3(0,0,1).MultiplyNormal(box.m_tm);
	normals[ 5] = Vector3(1,0,0).MultiplyNormal(box.m_tm);

	Vector3 vertices1[8] = {
		Vector3(m_min.x, m_max.y, m_min.z), 
		Vector3(m_max.x, m_max.y, m_min.z), 
		Vector3(m_min.x, m_min.y, m_min.z), 
		Vector3(m_max.x, m_min.y, m_min.z),
		Vector3(m_min.x, m_max.y, m_max.z), 
		Vector3(m_max.x, m_max.y, m_max.z),
		Vector3(m_min.x, m_min.y, m_max.z), 
		Vector3(m_max.x, m_min.y, m_max.z),
	};
	for (u_int i=0; i < 8; ++i)
		vertices1[ i] *= m_tm;


	Vector3 vertices2[8] = {
		Vector3(box.m_min.x, box.m_max.y, box.m_min.z), 
		Vector3(box.m_max.x, box.m_max.y, box.m_min.z), 
		Vector3(box.m_min.x, box.m_min.y, box.m_min.z), 
		Vector3(box.m_max.x, box.m_min.y, box.m_min.z),
		Vector3(box.m_min.x, box.m_max.y, box.m_max.z), 
		Vector3(box.m_max.x, box.m_max.y, box.m_max.z),
		Vector3(box.m_min.x, box.m_min.y, box.m_max.z), 
		Vector3(box.m_max.x, box.m_min.y, box.m_max.z),
	};
	for (u_int i=0; i < 8; ++i)
		vertices2[ i] *= box.m_tm;


	for( u_int i = 0 ; i < 6; i++ )
	{
		float shape1Min, shape1Max, shape2Min, shape2Max ;
		SATtest( normals[ i], vertices1, shape1Min, shape1Max ) ;
		SATtest( normals[i], vertices2, shape2Min, shape2Max ) ;
		if (!overlaps(shape1Min, shape1Max, shape2Min, shape2Max))
		{
			return false; // NO INTERSECTION
		}
	}

	return true;
}


// 피킹 되었다면 true를 리턴한다.
// orig, dir : ray 값.
bool cBoundingBox::Pick(const Vector3 &orig, const Vector3 &dir
, const Matrix44 &tm // = Matrix44::Identity
)
{
	const Matrix44 transform = m_tm * tm;
	Vector3 vertices[8] = {
		Vector3(m_max.x, m_max.y, m_min.z),
		Vector3(m_min.x, m_max.y, m_min.z),
		Vector3(m_max.x, m_max.y, m_max.z),
		Vector3(m_min.x, m_max.y, m_max.z),
		Vector3(m_max.x, m_min.y, m_min.z),
		Vector3(m_min.x, m_min.y, m_min.z), 
		Vector3(m_max.x, m_min.y, m_max.z),
		Vector3(m_min.x, m_min.y, m_max.z),
	};
	for (u_int i=0; i < 8; ++i)
		vertices[ i] *= transform;

	//       3-----2
	//      /|      /|
	//     1-----0 |
	//	   | 7 -  | 6
	//	   |/      |/
	//	   5-----4
	Triangle triangle[ 12] = {
	// top
	Triangle(vertices[0], vertices[1], vertices[2]),
	Triangle(vertices[1], vertices[3], vertices[2]),
	// left
	Triangle(vertices[1], vertices[5], vertices[7]),
	Triangle(vertices[3], vertices[1], vertices[7]),
	// right
	Triangle(vertices[0], vertices[2], vertices[6]),
	Triangle(vertices[0], vertices[6], vertices[4]),
	// front
	Triangle(vertices[2], vertices[3], vertices[6]),
	Triangle(vertices[3], vertices[7], vertices[6]),
	// back
	Triangle(vertices[1], vertices[0], vertices[4]),
	Triangle(vertices[1], vertices[4], vertices[5]),
	// bottom
	Triangle(vertices[4], vertices[7], vertices[5]),
	Triangle(vertices[4], vertices[6], vertices[7]),
	};

	for (int i=0; i < 12; ++i)
	{
		float fT, fU, fV;
		if (triangle[i].Intersect(orig, dir, &fT, &fU, &fV))
			return true;
	}

	return false;
}


// 피킹 되었다면 true를 리턴한다.
// orig, dir : ray 값.
bool cBoundingBox::Pick2(const Vector3 &orig, const Vector3 &dir, float *pfT, float *pfU, float *pfV)
{
	Vector3 vertices[8] = {
		Vector3(m_max.x, m_max.y, m_min.z),
		Vector3(m_min.x, m_max.y, m_min.z),
		Vector3(m_max.x, m_max.y, m_max.z),
		Vector3(m_min.x, m_max.y, m_max.z),
		Vector3(m_max.x, m_min.y, m_min.z),
		Vector3(m_min.x, m_min.y, m_min.z),
		Vector3(m_max.x, m_min.y, m_max.z),
		Vector3(m_min.x, m_min.y, m_max.z),
	};
	for (u_int i = 0; i < 8; ++i)
		vertices[i] *= m_tm;

	//       3-----2
	//      /|      /|
	//     1-----0 |
	//	   | 7 -  | 6
	//	   |/      |/
	//	   5-----4
	Triangle triangle[12] = {
		// top
		Triangle(vertices[0], vertices[1], vertices[2]),
		Triangle(vertices[1], vertices[3], vertices[2]),
		// left
		Triangle(vertices[1], vertices[5], vertices[7]),
		Triangle(vertices[3], vertices[1], vertices[7]),
		// right
		Triangle(vertices[0], vertices[2], vertices[6]),
		Triangle(vertices[0], vertices[6], vertices[4]),
		// front
		Triangle(vertices[2], vertices[3], vertices[6]),
		Triangle(vertices[3], vertices[7], vertices[6]),
		// back
		Triangle(vertices[1], vertices[0], vertices[4]),
		Triangle(vertices[1], vertices[4], vertices[5]),
		// bottom
		Triangle(vertices[4], vertices[7], vertices[5]),
		Triangle(vertices[4], vertices[6], vertices[7]),
	};

	struct tri { int idx; float t, u, v; };
	tri tries[12];
	for (int i = 0; i < 12; ++i)
		tries[i].idx = -1;

	int n = 0;
	for (int i = 0; i < 12; ++i)
	{
		float fT, fU, fV;
		if (triangle[i].Intersect(orig, dir, &fT, &fU, &fV))
		{
			tries[n].idx = i;
			tries[n].t = fT;
			tries[n].u = fU;
			tries[n].v = fV;
			++n;
		}
		//return true;
	}

	if (n <= 0)
		return false;

	tri nearTri;
	nearTri.t = FLT_MAX;
	for (int i = 0; i < n; ++i)
	{
		if (nearTri.t < tries[i].t)
		{
			nearTri.idx = tries[i].idx;
			nearTri.t = tries[i].t;
			nearTri.u = tries[i].u;
			nearTri.v = tries[i].v;
		}
	}

	if (pfT)
		*pfT = nearTri.t;
	if (pfU)
		*pfU = nearTri.u;
	if (pfV)
		*pfV = nearTri.v;

	return true;
}


// 피킹 되었다면 true를 리턴한다.
// orig, dir : ray 값.
bool cBoundingBox::Pick3(const Vector3 &orig, const Vector3 &dir, float *pDistance
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	const Matrix44 transform = m_tm * tm;
	Vector3 vertices[8] = {
		Vector3(m_max.x, m_max.y, m_min.z),
		Vector3(m_min.x, m_max.y, m_min.z),
		Vector3(m_max.x, m_max.y, m_max.z),
		Vector3(m_min.x, m_max.y, m_max.z),
		Vector3(m_max.x, m_min.y, m_min.z),
		Vector3(m_min.x, m_min.y, m_min.z),
		Vector3(m_max.x, m_min.y, m_max.z),
		Vector3(m_min.x, m_min.y, m_max.z),
	};
	for (u_int i = 0; i < 8; ++i)
		vertices[i] *= transform;

	//       3-----2
	//      /|      /|
	//     1-----0 |
	//	   | 7 -  | 6
	//	   |/      |/
	//	   5-----4
	Triangle triangle[12] = {
		// top
		Triangle(vertices[0], vertices[1], vertices[2]),
		Triangle(vertices[1], vertices[3], vertices[2]),
		// left
		Triangle(vertices[1], vertices[5], vertices[7]),
		Triangle(vertices[3], vertices[1], vertices[7]),
		// right
		Triangle(vertices[0], vertices[2], vertices[6]),
		Triangle(vertices[0], vertices[6], vertices[4]),
		// front
		Triangle(vertices[2], vertices[3], vertices[6]),
		Triangle(vertices[3], vertices[7], vertices[6]),
		// back
		Triangle(vertices[1], vertices[0], vertices[4]),
		Triangle(vertices[1], vertices[4], vertices[5]),
		// bottom
		Triangle(vertices[4], vertices[7], vertices[5]),
		Triangle(vertices[4], vertices[6], vertices[7]),
	};

	int tri[12];
	int n = 0;
	for (int i = 0; i < 12; ++i)
	{
		if (triangle[i].Intersect(orig, dir))
			tri[n++] = i;
	}

	if (n <= 0)
		return false;

	if (n == 1)
	{
		if (pDistance)
			*pDistance = triangle[tri[0]].Distance(orig);
		return true;
	}

	int nearTri = tri[0];
	float nearLen = triangle[tri[0]].Distance(orig);
	for (int i = 1; i < n; ++i)
	{
		const float len = triangle[tri[i]].Distance(orig);
		if (nearLen > len)
		{
			nearTri = tri[i];
			nearLen = len;
		}
	}

	if (pDistance)
		*pDistance = nearLen;

	return true;
}


// 할당 연산자.
cBoundingBox& cBoundingBox::operator=(const cCube &cube)
{
	SetBoundingBox(cube.GetMin(), cube.GetMax());
	SetTransform(cube.GetTransform());
	return *this;
}


// 경계박스 중점을 리턴한다.
Vector3 cBoundingBox::Center() const
{
	return (m_max+m_min)/2.f;
}


void cBoundingBox::Scale(const Vector3 &scale)
{
	Vector3 center = Center();
	m_max -= center;
	m_min -= center;

	Matrix44 scaleT;
	scaleT.SetScale(scale);
	m_min *= scaleT;
	m_max *= scaleT;
	
	m_min += center;
	m_max += center;
}
