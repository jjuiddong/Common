
#include "stdafx.h"
#include "boundingbox.h"

using namespace common;


cBoundingBox::cBoundingBox()
{
	m_type = eCollisionType::BOX;
	SetBoundingBox(Vector3(0,0,0), Vector3(1,1,1), Quaternion(0,0,0,1));
}

//cBoundingBox::cBoundingBox(const cCube &cube)
//{
//	m_type = eCollisionType::BOX;
//	operator=(cube);
//}


cBoundingBox::cBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q )
{
	m_type = eCollisionType::BOX;
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
	for(uint i = 0 ; i < 8; i++ )
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


bool cBoundingBox::Collision(const cCollisionObj &obj
	, OUT Vector3 *outPos //= NULL
	, OUT float *distance //= NULL
) const
{
	switch (obj.m_type)
	{
	// OBB vs OBB 충돌처리.
	case eCollisionType::BOX:
	{
		if (const cBoundingBox *p = dynamic_cast<const cBoundingBox*>(&obj))
		{
			const ContainmentType result = m_bbox.Contains(p->m_bbox);
			switch (result)
			{
			case INTERSECTS:
			case CONTAINS:
				return true;
			}
		}
	}
	break;

	// OBB vs Sphere 충돌처리
	case eCollisionType::SPHERE:
	{
		if (const cBoundingSphere *p = dynamic_cast<const cBoundingSphere*>(&obj))
		{
			const ContainmentType result = m_bbox.Contains(p->m_bsphere);
			switch (result)
			{
			case INTERSECTS:
			case CONTAINS:
				return true;
			}
		}
	}
	break;

	default:
		assert(0);
		break;
	}

	return false;
}


// X-Y, Z-Y Plane Collision Test
// return Collision Position, Collision Plane
// collision Pos : boundingbox 면에 부딪친 위치
// collision Plane : 부딪친 boundingbox 면
// outVertexLen1, outVertexLen2 : 부딪친 면에, 충돌지점에서 좌우 꼭지점까지의 거리, (좌우 순서는 바뀔수 있다.)
bool cBoundingBox::Collision2D(cBoundingSphere &sphere
	, OUT Vector3 *outPos //= NULL
	, OUT Plane *outPlane //= NULL
	, OUT Vector3 *outVertex1 //= NULL
	, OUT Vector3 *outVertex2 //= NULL
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

	// 꼭지점에 부딪쳤을 때, 거리를 계산해 충돌여부를 따진다.
	if ((d1 * d2 > 0) && (d1 > 0))
	{
		const float len = sqrt(d1*d1 + d2*d2);
		if (len > sphere.GetRadius())
			return false;
	}

	// Sphere가 BoundingBox 내부에서 충돌 했을 때.
	if ((d1 * d2 > 0) && (d1 < 0))
	{
		// 가까운 평면으로 튀어 나간다. (Solid Mode)
		const bool isZy = abs(d1) < abs(d2);
		const float f1 = distZY >= 0 ? 1.f : -1.f;
		const float f2 = distXY >= 0 ? 1.f : -1.f;
		const float l1 = isZy ? f1*r1 : distZY;
		const float l2 = !isZy ? f2*r2 : distXY;
		const float n1 = isZy ? f1 : 0.f;
		const float n2 = !isZy ? f2 : 0.f;
		const float if1 = isZy ? f1 : -f1;
		const float if2 = !isZy ? f2 : -f2;

		const Vector3 vertexPos1 = Center() + (planeZY.N * f1 * r1) + (planeXY.N * f2 * r2);
		const Vector3 vertexPos2 = Center() + (planeZY.N * if1 * r1) + (planeXY.N * if2 * r2);
		const Vector3 collisionPos = Center() + (planeZY.N * l1) + (planeXY.N * l2);
		const Vector3 vertexN = ((planeZY.N * n1) + (planeXY.N * n2)).Normal();

		// Collision Position
		if (outPos)
			*outPos = collisionPos;
		if (outPlane)
			*outPlane = Plane(vertexN, vertexPos1);

		if (outVertex1 && outVertex2)
		{
			if ((n1 > 0) && (n2 > 0)) // 꼭지점에 부딪쳤을 때,
			{
				// 꼭지점을 중심으로 Sphere 반지름의 두배 만큼을 충돌면으로 한다.
				const Vector3 rightV = Vector3(0, 1, 0).CrossProduct(vertexN).Normal();
				*outVertex1 = collisionPos + rightV * sphere.GetRadius()*2.f;
				*outVertex2 = collisionPos + rightV * -sphere.GetRadius()*2.f;
			}
			else
			{
				*outVertex1 = vertexPos1;
				*outVertex2 = vertexPos2;
			}
		}
	}
	else
	{
		// X,Z 축을 따로계산한 후, 합산해서 최종 좌표를 구한다.
		const bool isZy = d1 >= 0.f;
		const float f1 = distZY >= 0 ? 1.f : -1.f;
		const float f2 = distXY >= 0 ? 1.f : -1.f;
		const float l1 = d1 > 0 ? f1*r1 : distZY;
		const float l2 = d2 > 0 ? f2*r2 : distXY;
		const float n1 = d1 > 0 ? f1 : 0.f;
		const float n2 = d2 > 0 ? f2 : 0.f;
		const float if1 = isZy ? f1 : -f1;
		const float if2 = !isZy ? f2 : -f2;

		const Vector3 vertexPos1 = Center() + (planeZY.N * f1 * r1) + (planeXY.N * f2 * r2);
		const Vector3 vertexPos2 = Center() + (planeZY.N * if1 * r1) + (planeXY.N * if2 * r2);
		const Vector3 collisionPos = Center() + (planeZY.N * l1) + (planeXY.N * l2);
		const Vector3 vertexN = ((planeZY.N * n1) + (planeXY.N * n2)).Normal();

		// Collision Position
		if (outPos)
			*outPos = collisionPos;
		if (outPlane)
			*outPlane = Plane(vertexN, collisionPos);

		if (outVertex1 && outVertex2)
		{
			if ((d1 > 0) && (d2 > 0)) // 꼭지점에 부딪쳤을 때,
			{
				// 꼭지점을 중심으로 Sphere 반지름의 0.1배 만큼을 충돌면으로 한다.
				const Vector3 rightV = Vector3(0,1,0).CrossProduct(vertexN).Normal();
				*outVertex1 = collisionPos + rightV * sphere.GetRadius() * 0.1f;
				*outVertex2 = collisionPos + rightV * -sphere.GetRadius() * 0.1f;
			}
			else
			{
				*outVertex1 = vertexPos1;
				*outVertex2 = vertexPos2;
			}
		}
	}

	return true;
}


// 피킹 되었다면 true를 리턴한다.
// orig, dir : ray 값.
bool cBoundingBox::Pick(const Vector3 &orig, const Vector3 &dir
	, OUT float *distance //=NULL
) const
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


//bool cBoundingBox::Pick(const Ray &ray
//	, OUT float *distance //= NULL
//) const
//{
//	return Pick(ray.orig, ray.dir, distance);
//}
bool cBoundingBox::Pick(const Ray &ray
	, OUT float *distance //= NULL
) const
{
	return Pick(ray.orig, ray.dir, distance);
}


// 할당 연산자.
//cBoundingBox& cBoundingBox::operator=(const cCube &cube)
//{
//	*this = cube.m_boundingBox;
//	*this *= cube.GetWorldMatrix();
//	return *this;
//}


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


float cBoundingBox::GetScale() const
{
	const Vector3 scale = *(Vector3*)&m_bbox.Extents;
	return scale.Length();
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
