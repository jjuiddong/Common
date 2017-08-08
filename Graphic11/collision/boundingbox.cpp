
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


cBoundingBox::cBoundingBox(const Vector3 &center, const Vector3 &scale
	, const Quaternion &q//=Quaternion()
)
{
	SetBoundingBox(center, scale, q);
}


void cBoundingBox::SetBoundingBox(const Vector3 &center, const Vector3 &scale
	, const Quaternion &q//=Quaternion()
)
{
	m_bbox = BoundingOrientedBox(*(XMFLOAT3*)&center, *(XMFLOAT3*)&scale, *(XMFLOAT4*)&q);
}


void cBoundingBox::SetBoundingBox(const Transform &tfm)
{
	m_bbox = BoundingOrientedBox(*(XMFLOAT3*)&tfm.pos, *(XMFLOAT3*)&tfm.scale, *(XMFLOAT4*)&tfm.rot);
}



void cBoundingBox::SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width)
{
	Vector3 v = p1 - p0;
	const float len = v.Length();
	v.Normalize();

	m_bbox.Extents = XMFLOAT3(width, width, len / 2.f);
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

	return false;
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
	m_bbox.Extents = *(XMFLOAT3*)&scale;
}


// return x,y,z dimension
Vector3 cBoundingBox::GetDimension() const 
{
	return *(Vector3*)&m_bbox.Extents;
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


XMMATRIX cBoundingBox::GetTransformXM() const
{
	XMVECTOR q = XMLoadFloat4(&m_bbox.Orientation);
	XMMATRIX rot = XMMatrixRotationQuaternion(q);
	XMMATRIX tm = XMMatrixScaling(m_bbox.Extents.x, m_bbox.Extents.y, m_bbox.Extents.z) * rot 
		* XMMatrixTranslation(m_bbox.Center.x, m_bbox.Center.y, m_bbox.Center.z);
	return tm;
}


Matrix44 cBoundingBox::GetTransform() const
{
	Matrix44 S;
	S.SetScale(Vector3(m_bbox.Extents.x, m_bbox.Extents.y, m_bbox.Extents.z));
	Quaternion q = *(Quaternion*)&m_bbox.Orientation;
	Matrix44 R = q.GetMatrix();
	Matrix44 T;
	T.SetTranslate(Vector3(m_bbox.Center.x, m_bbox.Center.y, m_bbox.Center.z));
	return S * R * T;
}


cBoundingBox cBoundingBox::operator * (const XMMATRIX &rhs) 
{
	BoundingOrientedBox bbox;
	m_bbox.Transform(bbox, rhs);
	cBoundingBox ret;
	ret.m_bbox = bbox;
	return ret;
}


const cBoundingBox& cBoundingBox::operator *= (const Matrix44 &rhs) 
{
	*this = operator*(rhs.GetMatrixXM());
	return *this;
}
