
#include "stdafx.h"
#include "frustum.h"

using namespace graphic;

cFrustum::cFrustum()
//:	m_fullCheck(false)
//,	m_plane(6) // 절두체 평면 6개
//, m_epsilon(5.f)
{
}

cFrustum::~cFrustum()
{
}


//-----------------------------------------------------------------------------//
// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
//-----------------------------------------------------------------------------//
bool cFrustum::SetFrustum(const Vector3 &pos, const Vector3 &direction, const Matrix44 &matProj)
{
	BoundingFrustum::CreateFromMatrix(m_frustum, matProj.GetMatrixXM());

	Matrix44 view;
	view.SetView(pos, direction, Vector3(0, 1, 0));
	Quaternion q = view.GetQuaternion();	
	m_frustum.Origin = *(XMFLOAT3*)&pos;
	m_frustum.Orientation = *(XMFLOAT4*)&q;
	m_viewProj = view * matProj;

	return TRUE;
}

bool cFrustum::SetFrustum(const cCamera &camera)
{
	return SetFrustum(camera.GetEyePos(), camera.GetDirection(), camera.GetProjectionMatrix());
}


//-----------------------------------------------------------------------------//
// 한점 point가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
//-----------------------------------------------------------------------------//
bool cFrustum::IsIn( const Vector3 &point ) const
{
	return DISJOINT != m_frustum.Contains(point.GetVectorXM());
}



bool cFrustum::IsInSphere(const cBoundingSphere &sphere) const
{
	return DISJOINT != m_frustum.Contains(sphere.m_bsphere);
}


bool cFrustum::IsInBox(const cBoundingBox &bbox) const
{
	return DISJOINT != m_frustum.Contains(bbox.m_bbox);

	//const Vector3 vertices[] = {
	//	bbox.m_min
	//	, Vector3(bbox.m_max.x, bbox.m_min.y, bbox.m_min.z)
	//	, Vector3(bbox.m_min.x, bbox.m_max.y, bbox.m_min.z)
	//	, Vector3(bbox.m_min.x, bbox.m_max.y, bbox.m_max.z)
	//	, bbox.m_max
	//	, Vector3(bbox.m_min.x, bbox.m_max.y, bbox.m_max.z)
	//	, Vector3(bbox.m_max.x, bbox.m_min.y, bbox.m_max.z)
	//	, Vector3(bbox.m_max.x, bbox.m_max.y, bbox.m_min.z)
	//};

	//for (int i = 0; i < 8; ++i)
	//{
	//	if (IsIn(vertices[i]))
	//		return true;
	//}

	//Vector3 size = bbox.m_max - bbox.m_min;
	//const float radius = size.Length() * 0.5f;
	//return IsInSphere(bbox.Center() * tm, radius);
	return false;
}


// Split Frustum
// f : 0 ~ 1
void cFrustum::Split2(cCamera &cam, const float f1, const float f2
	, cFrustum &out1, cFrustum &out2)
{
	//const float oldNearPlane = cam.m_nearPlane;
	//const float oldFarPlane = cam.m_farPlane;
	//const float far1 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f1);
	//const float far2 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f2); 

	//cam.ReCalcProjection(cam.m_nearPlane, far1);
	//out1.SetFrustum(cam.GetViewProjectionMatrix());
	//
	//cam.ReCalcProjection(far1, far2);
	//out2.SetFrustum(cam.GetViewProjectionMatrix());

	//// recovery
	//cam.ReCalcProjection(oldNearPlane, oldFarPlane);
}


// Split Frustum
// f : 0 ~ 1
void cFrustum::Split3(cCamera &cam, const float f1, const float f2, const float f3
	, cFrustum &out1, cFrustum &out2, cFrustum &out3)
{
	//const float oldNearPlane = cam.m_nearPlane;
	//const float oldFarPlane = cam.m_farPlane;
	//const float far1 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f1);
	//const float far2 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f2);
	//const float far3 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f3);

	//cam.ReCalcProjection(cam.m_nearPlane, far1);
	//out1.SetFrustum(cam.GetViewProjectionMatrix());

	//cam.ReCalcProjection(far1, far2);
	//out2.SetFrustum(cam.GetViewProjectionMatrix());

	//cam.ReCalcProjection(far2, far3);
	//out3.SetFrustum(cam.GetViewProjectionMatrix());

	//// recovery
	//cam.ReCalcProjection(oldNearPlane, oldFarPlane);
}


// return Frustum & Plane Collision Vertex
// Plane is Usually Ground Plane
void cFrustum::GetGroundPlaneVertices(const Plane &plane, OUT Vector3 outVertices[4]) const
{
	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	//Vector3 vertices[8] = {
	//	Vector3(-1,1,0), Vector3(1,1,0), Vector3(-1,-1,0), Vector3(1,-1,0),
	//	Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	//};
	//Matrix44 matInv = m_viewProj.Inverse();
	//for (int i = 0; i < 8; ++i)
	//	vertices[i] *= matInv;

	//// Far Plane
	//Vector3 p0, p1;
	//if (plane.LineCross(vertices[0], vertices[4], &p0) == 0)
	//	p0 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//if (plane.LineCross(vertices[1], vertices[5], &p1) == 0)
	//	p1 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//Vector3 p4 = plane.Pick(vertices[4], (vertices[6] - vertices[4]).Normal());
	//Vector3 p5 = plane.Pick(vertices[5], (vertices[7] - vertices[5]).Normal());

	//// Near Plane
	//Vector3 p2, p3;
	//if (plane.LineCross(vertices[3], vertices[7], &p2) == 0)
	//	p2 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//if (plane.LineCross(vertices[2], vertices[6], &p3) == 0)
	//	p3 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//Vector3 p6 = plane.Pick(vertices[1], (vertices[3] - vertices[1]).Normal());
	//Vector3 p7 = plane.Pick(vertices[0], (vertices[2] - vertices[0]).Normal());

	//outVertices[0] = (LengthRoughly(p0) < LengthRoughly(p4)) ? p0 : p4;
	//outVertices[1] = (LengthRoughly(p1) < LengthRoughly(p5)) ? p1 : p5;
	//outVertices[2] = (LengthRoughly(p2) < LengthRoughly(p6)) ? p2 : p6;
	//outVertices[3] = (LengthRoughly(p3) < LengthRoughly(p7)) ? p3 : p7;
}


float cFrustum::LengthRoughly(const Vector3 &pos) const
{
	return (*(Vector3*)&m_frustum.Origin).LengthRoughly(pos);
}


cFrustum& cFrustum::operator=(const cFrustum &rhs)
{
	if (this != &rhs)
	{
		m_viewProj = rhs.m_viewProj;
		m_frustum = rhs.m_frustum;
	}
	return *this;
}
