
#include "stdafx.h"
#include "boundingsphere.h"

using namespace common;


cBoundingSphere::cBoundingSphere() 
{
	m_type = eCollisionType::SPHERE;
}

cBoundingSphere::cBoundingSphere(const Vector3 &center, const float radius) 
	: m_bsphere(*(XMFLOAT3*)&center, radius) 
{
	m_type = eCollisionType::SPHERE;
}

cBoundingSphere::~cBoundingSphere() 
{
}


void cBoundingSphere::SetBoundingSphere(const Vector3 &center, const float radius) 
{
	m_bsphere.Center = *(XMFLOAT3*)&center;
	m_bsphere.Radius = radius;
}


void cBoundingSphere::SetBoundingSphere(const cBoundingBox &bbox
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	BoundingSphere::CreateFromBoundingBox(m_bsphere, bbox.m_bbox);
}


void cBoundingSphere::SetPos(const Vector3 &pos) 
{
	m_bsphere.Center = *(XMFLOAT3*)&pos;
}


const Vector3& cBoundingSphere::GetPos() const 
{
	return *(Vector3*)&m_bsphere.Center;
}


void cBoundingSphere::SetRadius(const float radius) 
{
	m_bsphere.Radius = radius;
}


float cBoundingSphere::GetRadius() const 
{
	return m_bsphere.Radius;
}


bool cBoundingSphere::Collision(const cCollisionObj &obj
	, OUT Vector3 *outPos //= NULL
	, OUT float *distance //= NULL
) const
{
	switch (obj.m_type)
	{
	case eCollisionType::SPHERE:
		if (const cBoundingSphere *p = dynamic_cast<const cBoundingSphere*>(&obj))
			return Intersects(*p);
		break;

	case eCollisionType::BOX:
		if (const cBoundingBox *p = dynamic_cast<const cBoundingBox*>(&obj))
		{
			const ContainmentType result = p->m_bbox.Contains(m_bsphere);
			switch (result)
			{
			case INTERSECTS:
			case CONTAINS:
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


bool cBoundingSphere::Pick(const Ray &ray
	, OUT float *distance //= NULL
) const
{
	return Intersects(ray, distance);
}


Transform cBoundingSphere::GetTransform() const
{
	Transform tfm;
	tfm.pos = Vector3(m_bsphere.Center.x, m_bsphere.Center.y, m_bsphere.Center.z);
	tfm.scale = Vector3(1, 1, 1)*m_bsphere.Radius;
	return tfm;
}


bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0.f) return false;
	else if (discr == 0.f) x0 = x1 = -0.5f * b / a;
	else {
		float q = (b > 0.f) ?
			-0.5f * (b + sqrt(discr)) :
			-0.5f * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);

	return true;
}


// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool cBoundingSphere::Intersects(const Ray &ray
	, OUT float *distance //= NULL
) const 
{
	const Vector3 center = GetPos();
	const float radius = GetRadius();
	const Line line(ray.orig, ray.dir * 10000.f);
	const float d = line.GetDistance(center);
	if (radius < d)
		return false;

	if (distance)
	{
		const Vector3 pos = line.Projection(center);
		*distance = pos.Distance(ray.orig);
	}
	return true;	
}


// Ray가 radius 반지름을 가진 원기둥이라고 가정하고,
// 충돌 계산을 한다. x축으로만 확장해서 검사한다.
bool cBoundingSphere::Intersects(const Ray &ray
	, const float radius
	, OUT float *distance //= NULL
) const
{
	const Vector3 right = Vector3(0, 1, 0).CrossProduct(ray.dir).Normal();
	const Ray rayAr[3] = {
		ray
		, Ray(ray.orig + (right * radius), ray.dir)
		, Ray(ray.orig - (right * radius), ray.dir)
	};
	float dist[3] = {
		FLT_MAX, FLT_MAX, FLT_MAX
	};

	float minDist = FLT_MAX;
	int idx = -1;
	for (int i = 0; i < ARRAYSIZE(rayAr); ++i)
	{
		float t;
		if (Intersects(rayAr[i], &t))
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

	if (distance)
		*distance = minDist;

	return true;
}


bool cBoundingSphere::Intersects(const cBoundingSphere &bspere) const 
{
	return m_bsphere.Intersects(bspere.m_bsphere);
}


cBoundingSphere cBoundingSphere::operator * (const Transform &rhs) 
{
	Matrix44 m;
	m.SetScale(rhs.scale);
	const Vector3 radius = Vector3(m_bsphere.Radius, 0, 0) * m;
	return cBoundingSphere(*(Vector3*)&m_bsphere.Center * rhs, radius.x);
}


cBoundingSphere cBoundingSphere::operator * (const Matrix44 &rhs) 
{
	const float scale = rhs.GetScale().Length();
	return cBoundingSphere(*(Vector3*)&m_bsphere.Center * rhs, m_bsphere.Radius*scale);
}


cBoundingSphere cBoundingSphere::operator * (const XMMATRIX &rhs) 
{
	const XMVECTOR center = XMLoadFloat3(&m_bsphere.Center);
	const XMVECTOR pos = XMVector3Transform(center, rhs);
	XMFLOAT3 outPos;
	XMStoreFloat3(&outPos, pos);

	XMVECTOR _t, _s, _q;
	XMMatrixDecompose(&_s, &_q, &_t, rhs);
	//_s = XMVector3Length(_s);

	XMFLOAT3 outScale;
	XMStoreFloat3(&outScale, _s);
	XMMATRIX mScale = XMMatrixScaling(outScale.x, outScale.y, outScale.z);
	XMVECTOR vRadius = XMLoadFloat3((XMFLOAT3*)&Vector3(m_bsphere.Radius, 0, 0));
	const XMVECTOR radius = XMVector3Transform(vRadius, mScale);

	XMFLOAT3 retRadius;
	XMStoreFloat3(&retRadius, radius);

	//return cBoundingSphere(*(Vector3*)&outPos, m_bsphere.Radius*outScale.x);
	return cBoundingSphere(*(Vector3*)&outPos, retRadius.x);
}


const cBoundingSphere& cBoundingSphere::operator *= (const Matrix44 &rhs) 
{
	*(Vector3*)&m_bsphere.Center *= rhs;
	// no radius calculate
	return *this;
}


XMMATRIX cBoundingSphere::GetTransformXM() const 
{
	XMMATRIX tm = XMMatrixScaling(m_bsphere.Radius, m_bsphere.Radius, m_bsphere.Radius)
		* XMMatrixTranslation(m_bsphere.Center.x, m_bsphere.Center.y, m_bsphere.Center.z);
	return tm;
}
