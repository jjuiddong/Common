
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


bool cBoundingSphere::Intersects(const Ray &ray
	, OUT float *distance //= NULL
) const 
{
	//const XMVECTOR dir = XMLoadFloat3((XMFLOAT3*)&ray.dir);
	//const XMVECTOR orig = XMLoadFloat3((XMFLOAT3*)&ray.orig);
	//float dist;
	//const bool result = m_bsphere.Intersects(orig, dir, dist);
	//if (distance)
	//	*distance = dist;
	//return result;


	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	//// Solve quadratic equation
	//const Vector3 _point = ray.orig;
	//const Vector3 _direction = ray.dir;
	//const Vector3 center = GetPos();
	//const float radius = GetRadius();

	//float a = _direction.Length();
	//if (a == 0.0)  return false;
	//float b = 2.0f * (_point.DotProduct(_direction) - _direction.DotProduct(center));
	//Vector3 tempDiff = center - _point;
	//float c = tempDiff.Length() - (radius*radius);
	//float disc = b * b - 4 * a * c;
	//if (disc < 0.0f)
	//	return false;

	//int numIntersections;
	//if (disc == 0.0f)
	//	numIntersections = 1;
	//else
	//	numIntersections = 2;
	//return numIntersections;



	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	const Vector3 center = GetPos();
	const float radius2 = GetRadius() * GetRadius();

	float t0, t1; // solutions for t if the ray intersects 
#if 0
	// geometric solution
	Vector3 L = center - ray.orig;
	float tca = L.DotProduct(ray.dir);
	// if (tca < 0) return false;
	float d2 = L.DotProduct(L) - tca * tca;
	if (d2 > radius2) return false;
	float thc = sqrt(radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;
#else 
	// analytic solution
	Vector3 L = ray.orig - center;
	float a = ray.dir.DotProduct(ray.dir);
	float b = 2 * ray.dir.DotProduct(L);
	float c = L.DotProduct(L) - radius2;
	if (!solveQuadratic(a, b, c, t0, t1)) return false;
#endif 
	if (t0 > t1) 
		std::swap(t0, t1);

	if (t0 < 0) 
	{
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if (t0 < 0) return false; // both t0 and t1 are negative 
	}

	if (distance)
		*distance = t0;

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
