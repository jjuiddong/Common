
#include "stdafx.h"
#include "boundingbox.h"

using namespace graphic;


cBoundingBox::cBoundingBox()
{
	SetBoundingBox(Vector3(0,0,0), Vector3(1,1,1), Quaternion(0,0,0,1));
}

cBoundingBox::cBoundingBox(const cCube &cube)
{
	operator=(cube);
}


cBoundingBox::cBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q )
{
	SetBoundingBox(center, scale, q);
}


void cBoundingBox::SetBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q)
{
	m_bbox = BoundingOrientedBox(*(XMFLOAT3*)&center, *(XMFLOAT3*)&scale, *(XMFLOAT4*)&q);
}


void cBoundingBox::SetBoundingBox(const Transform &tfm)
{
	m_bbox = BoundingOrientedBox(*(XMFLOAT3*)&tfm.pos, *(XMFLOAT3*)&tfm.scale, *(XMFLOAT4*)&tfm.rot);
}


void cBoundingBox::SetBoundingBox(const sMinMax &minMax)
{
	const Vector3 center = (minMax._min + minMax._max) / 2;
	const Vector3 scale = Vector3(abs(minMax._min.x - minMax._max.x)*0.5f
		, abs(minMax._min.y - minMax._max.y)*0.5f
		, abs(minMax._min.z - minMax._max.z)*0.5f);
	SetBoundingBox(center, scale, Quaternion());
}


void cBoundingBox::SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	m_bbox.Extents = XMFLOAT3(width, width, len/2.f);
	m_bbox.Center = *(XMFLOAT3*)&((p0 + p1) / 2.f);

	Quaternion q(Vector3(0, 0, 1), v);
	m_bbox.Orientation = *(XMFLOAT4*)&q;
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
	const ContainmentType result = m_bbox.Contains(box.m_bbox);
	switch (result)
	{
	case INTERSECTS:
	case CONTAINS:
		return true;
	}
	return false;
}


// OBB vs Sphere 충돌처리
bool cBoundingBox::Collision(cBoundingSphere &sphere)
{
	const ContainmentType result = m_bbox.Contains(sphere.m_bsphere);
	switch (result)
	{
	case INTERSECTS:
	case CONTAINS:
		return true;
	}
	return false;
}


// X-Y, Z-Y Plane Collision Test
// return Collision Position
bool cBoundingBox::Collision2D(cBoundingSphere &sphere
	, OUT Vector3 *out //= NULL
)
{
	const Quaternion &q = *(Quaternion*)&m_bbox.Orientation;
	const Vector3 zyN = Vector3(1, 0, 0) * q; 
	Plane planeZY(zyN, Center());
	const float distZY = planeZY.Collision(sphere.GetPos());
	const float r1 = m_bbox.Extents.x;

	const Vector3 xyN = Vector3(0, 0, 1) * q;
	Plane planeXY(xyN, Center());
	const float distXY = planeXY.Collision(sphere.GetPos());
	const float r2 = m_bbox.Extents.z;

	const float d1 = (abs(distZY) - r1);
	const float d2 = (abs(distXY) - r2);
	if (d1 > sphere.GetRadius())
		return false;
	if (d2 > sphere.GetRadius())
		return false;

	const bool isZy = abs(d1) < abs(d2);
	const float dist = isZy? abs(distZY) : abs(distXY);
	const float r = isZy ? r1 : r2;

	const Vector3 N = isZy ? zyN : xyN;
	const float minDist = isZy ? distZY : distXY;
	const float rr = (minDist < 0) ? -(dist - r) : dist - r;

	// Collision Position
	if (out)
		*out = sphere.GetPos() + -N * rr;

	return true;
}


// 피킹 되었다면 true를 리턴한다.
// orig, dir : ray 값.
bool cBoundingBox::Pick(const Vector3 &orig, const Vector3 &dir
	, OUT float *distance //=NULL
)
{
	XMVECTOR o = XMLoadFloat3((XMFLOAT3*)&orig);
	XMVECTOR d = XMLoadFloat3((XMFLOAT3*)&dir);
	float dist = 0;
	if (!m_bbox.Intersects(o, d, dist))
		return false;

	if (distance)
		*distance = dist;

	return true;
}


bool cBoundingBox::Pick(const Ray &ray
	, OUT float *distance //= NULL
)
{
	return Pick(ray.orig, ray.dir, distance);
}


// 할당 연산자.
cBoundingBox& cBoundingBox::operator=(const cCube &cube)
{
	*this = cube.m_boundingBox;
	*this *= cube.GetWorldMatrix();
	return *this;
}


// 경계박스 중점을 리턴한다.
Vector3 cBoundingBox::Center() const
{
	return *(Vector3*)&m_bbox.Center;
}


void cBoundingBox::Scale(const Vector3 &scale)
{
	//m_bbox.Extents = *(XMFLOAT3*)&scale;
	m_bbox.Extents.x *= scale.x;
	m_bbox.Extents.y *= scale.y;
	m_bbox.Extents.z *= scale.z;
}


// return x,y,z dimension
Vector3 cBoundingBox::GetDimension() const 
{
	Vector3 scale = *(Vector3*)&m_bbox.Extents;
	Quaternion q = *(Quaternion*)&m_bbox.Orientation;
	scale *= q.GetMatrix();
	return scale * 2.f;
	//return (*(Vector3*)&m_bbox.Extents) * 2.f;
}


//void cBoundingBox::MultiyplyTransform(const Matrix44 &tm)
//{
//	XMVECTOR center = XMLoadFloat3(&m_bbox.Center);
//	XMVECTOR pos = XMLoadFloat3((XMFLOAT3*)&tm.GetPosition());
//	center += pos;
//
//	XMVECTOR extents = XMLoadFloat3(&m_bbox.Extents);
//	XMVECTOR scale = XMLoadFloat3((XMFLOAT3*)&tm.GetScale());
//	extents *= scale;
//
//	XMVECTOR q = XMLoadFloat4(&m_bbox.Orientation);
//	XMMATRIX rot = XMMatrixRotationQuaternion(q);
//	XMMATRIX rot2 = XMLoadFloat4x4((XMFLOAT4X4*)&tm);
//
//	rot *= rot2;
//	q = XMQuaternionRotationMatrix(rot);
//
//	XMStoreFloat3(&m_bbox.Center, center);
//	XMStoreFloat3(&m_bbox.Extents, extents);
//	XMStoreFloat4(&m_bbox.Orientation, q);
//}


XMMATRIX cBoundingBox::GetMatrixXM() const
{
	XMVECTOR q = XMLoadFloat4(&m_bbox.Orientation);
	XMMATRIX rot = XMMatrixRotationQuaternion(q);
	XMMATRIX tm = XMMatrixScaling(m_bbox.Extents.x, m_bbox.Extents.y, m_bbox.Extents.z) * rot 
		* XMMatrixTranslation(m_bbox.Center.x, m_bbox.Center.y, m_bbox.Center.z);
	return tm;
}


Matrix44 cBoundingBox::GetMatrix() const
{
	Matrix44 S;
	S.SetScale(Vector3(m_bbox.Extents.x, m_bbox.Extents.y, m_bbox.Extents.z));
	Quaternion q = *(Quaternion*)&m_bbox.Orientation;
	Matrix44 R = q.GetMatrix();
	Matrix44 T;
	T.SetTranslate(Vector3(m_bbox.Center.x, m_bbox.Center.y, m_bbox.Center.z));
	return S * R * T;
}


Transform cBoundingBox::GetTransform() const
{
	Transform tfm;
	tfm.pos = *(Vector3*)&m_bbox.Center;
	tfm.scale = *(Vector3*)&m_bbox.Extents;
	tfm.rot = *(Quaternion*)&m_bbox.Orientation;
	return tfm;
}


// 인자로 들어온 pos 값이 항상 경계박스 안에 있도록 계산해서 리턴한다.
Vector3 cBoundingBox::GetBoundingPoint(const Vector3 &pos) const
{
	Vector3 reVal;

	const Vector3 center = *(Vector3*)&m_bbox.Center;
	const Vector3 extents = *(Vector3*)&m_bbox.Extents;
	const Vector3 _min = center - extents;
	const Vector3 _max = center + extents;

	reVal = _min.Maximum(pos);
	reVal = _max.Minimum(reVal);
	return reVal;
}


// 8개의 꼭지점을 리턴한다.
// Returns the 8 vertices.
void cBoundingBox::GetVertexPoint(OUT Vector3 out[8]) const
{
	Vector3 vertices[8] = {
		Vector3(-1,1,-1)
		, Vector3(1,1,-1)
		, Vector3(-1,-1,-1)
		, Vector3(1,-1,-1)
		, Vector3(-1,1, 1)
		, Vector3(1,1, 1)
		, Vector3(-1,-1,1)
		, Vector3(1,-1,1)
	};

	Matrix44 tm = GetMatrix();
	for (int i = 0; i < 8; ++i)
		out[i] = vertices[i] * tm;
}


cBoundingBox cBoundingBox::operator * (const XMMATRIX &rhs)
{
	// BoundingOrientedBox::Transform() 함수는 x,y,z Scaling이 동일할때 써야 한다.
	// 이 코드는 BoundingBox의 pos,scale,rot 를 행렬처럼 이용해서 변환한다.
	// 2017-11-20, jjuiddong

	Transform tfm1;
	tfm1.pos = *(Vector3*)&m_bbox.Center;
	tfm1.scale = *(Vector3*)&m_bbox.Extents;
	tfm1.rot = *(Quaternion*)&m_bbox.Orientation;

	XMMATRIX m = tfm1.GetMatrixXM() * rhs;
	Transform temp = m;

	BoundingOrientedBox bbox;
	bbox.Center = *(XMFLOAT3*)&temp.pos;
	bbox.Extents = *(XMFLOAT3*)&temp.scale;
	bbox.Orientation = *(XMFLOAT4*)&temp.rot;

	cBoundingBox ret;
	ret.m_bbox = bbox;
	return ret;
}


cBoundingBox cBoundingBox::operator * (const Matrix44 &rhs)
{
	cBoundingBox bbox = *this;
	bbox *= rhs;
	return bbox;
}


cBoundingBox& cBoundingBox::operator *= (const Matrix44 &rhs) 
{
	*this = operator*(rhs.GetMatrixXM());
	return *this;
}

cBoundingBox& cBoundingBox::operator *= (const XMMATRIX &rhs)
{
	*this = operator*(rhs);
	return *this;
}


// Add Two BoundingBox
// return AABB Box
cBoundingBox cBoundingBox::operator + (const cBoundingBox &rhs)
{
	Vector3 vertices[16];
	GetVertexPoint(vertices);
	rhs.GetVertexPoint(vertices + 8);

	sMinMax minMax;
	for (int i = 0; i < 16; ++i)
		minMax.Update(vertices[i]);

	cBoundingBox ret;
	ret.SetBoundingBox(minMax);
	return ret;
}


cBoundingBox& cBoundingBox::operator += (const cBoundingBox &rhs)
{
	if (this != &rhs)
		*this = operator+(rhs);
	return *this;
}
