
#include "stdafx.h"
#include "frustum.h"

using namespace graphic;

cFrustum::cFrustum()
:	m_fullCheck(false)
,	m_plane(6) // 절두체 평면 6개
, m_epsilon(5.f)
{
}

cFrustum::~cFrustum()
{
}


//-----------------------------------------------------------------------------//
// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
//-----------------------------------------------------------------------------//
bool cFrustum::SetFrustum(const Matrix44 &matViewProj)
{
	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[8] = {
		Vector3(-1,1,0), Vector3(1,1,0), Vector3(-1,-1,0), Vector3(1,-1,0),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};

	// view * proj의 역행렬을 구한다.
	Matrix44 matInv = matViewProj.Inverse();

	// Vertex_최종 = Vertex_local * Matrix_world * Matrix_view * Matrix_Proj 인데,
	// Vertex_world = Vertex_local * Matrix_world이므로,
	// Vertex_최종 = Vertex_world * Matrix_view * Matrix_Proj 이다.
	// Vertex_최종 = Vertex_world * ( Matrix_view * Matrix_Proj ) 에서
	// 역행렬( Matrix_view * Matrix_Proj )^-1를 양변에 곱하면
	// Vertex_최종 * 역행렬( Matrix_view * Matrix_Proj )^-1 = Vertex_World 가 된다.
	// 그러므로, m_Vtx * matInv = Vertex_world가 되어, 월드좌표계의 프러스텀 좌표를 얻을 수 있다.
	//m_fullCheck = false;

	for (int i = 0; i < 8; i++)
		vertices[i] *= matInv;

	// 0번과 3번은 프러스텀중 near평면의 좌측상단과 우측하단이므로, 둘의 좌표를 더해서 2로 나누면
	// 카메라의 좌표를 얻을 수 있다.(정확히 일치하는 것은 아니다.)
	m_pos = ( vertices[0] + vertices[3] ) / 2.0f;

	// 얻어진 월드좌표로 프러스텀 평면을 만든다
	// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
	m_plane[0].Init(vertices[0], vertices[1], vertices[2]);	// 근 평면(near)
	m_plane[1].Init(vertices[0], vertices[4], vertices[1]);	// 위 평면(up)
	m_plane[2].Init(vertices[2], vertices[3], vertices[7]);	// 아래 평면(down)
	m_plane[3].Init( vertices[ 4], vertices[ 6], vertices[ 5] );	// 원 평면(far)
	m_plane[4].Init( vertices[ 0], vertices[ 2], vertices[ 6] );	// 좌 평면(left)
	m_plane[5].Init( vertices[ 1], vertices[ 5], vertices[ 7] );	// 우 평면(right)

	m_viewProj = matViewProj;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 정육면체의 minimum pos 와 maximum pos 로 절두체를 만든다.
//-----------------------------------------------------------------------------//
bool cFrustum::SetFrustum(const Vector3 &_min, const Vector3 &_max)
{
	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[8] = {
		Vector3(_min.x, _max.y, _min.z), Vector3(_max.x,_max.y,_min.z), Vector3(_min.x,_min.y,_min.z), Vector3(_max.x,_min.y,_min.z),
		Vector3(_min.x, _max.y, _max.z), Vector3(_max.x,_max.y,_max.z), Vector3(_min.x,_min.y,_max.z), Vector3(_max.x,_min.y,_max.z),
	};

	m_fullCheck = true;

	m_pos = (_min + _max) / 2.0f;

	// 얻어진 월드좌표로 프러스텀 평면을 만든다
	// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
	m_plane[0].Init(vertices[0], vertices[1], vertices[2]);	// 근 평면(near)
	m_plane[1].Init(vertices[0], vertices[4], vertices[1]);	// 위 평면(up)
	m_plane[2].Init(vertices[2], vertices[3], vertices[7]);	// 아래 평면(down)
	m_plane[3].Init(vertices[4], vertices[6], vertices[5]);	// 원 평면(far)
	m_plane[4].Init(vertices[0], vertices[2], vertices[6]);	// 좌 평면(left)
	m_plane[5].Init(vertices[1], vertices[5], vertices[7]);	// 우 평면(right)

	return true;
}


//-----------------------------------------------------------------------------//
// 한점 point가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
//-----------------------------------------------------------------------------//
bool cFrustum::IsIn( const Vector3 &point ) const
{
	for (int i=0; i < 6; ++i)
	{
		// m_fullCheck 가 false 라면 near, top, bottom  평면 체크는 제외 된다.
		if (!m_fullCheck && (i < 3))
			continue;

		const float dist = m_plane[ i].Distance( point );
		if (dist > m_epsilon)
			return false;
	}

	return true;
}


//-----------------------------------------------------------------------------//
// 중심(point)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
// TRUE를 반환, 아니면 FALSE를 반환한다.
//-----------------------------------------------------------------------------//
bool cFrustum::IsInSphere( const Vector3 &point, float radius
	, const Matrix44 &tm //= Matrix44::Identity
) const
{
	const Vector3 pos = point * tm;

	for (int i=0; i < 6; ++i)
	{
		// m_fullCheck 가 false 라면 near, top, bottom  평면 체크는 제외 된다.
		if (!m_fullCheck && (i < 3))
			continue;

		// 평면과 중심점의 거리가 반지름보다 크면 프러스텀에 없음
		const float dist = m_plane[ i].Distance( pos );
		if (dist > (radius+ m_epsilon))
			return false;
	}

	return true;
}


bool cFrustum::IsInSphere(const cBoundingSphere &sphere
	, const Matrix44 &tm //= Matrix44::Identity
) const
{
	return IsInSphere(sphere.m_pos, sphere.m_radius, tm);
}


bool cFrustum::IsInBox(const cBoundingBox &bbox) const
{
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

	Vector3 size = bbox.m_max - bbox.m_min;
	const float radius = size.Length() * 0.5f;
	return IsInSphere(bbox.Center(), radius);
}


// Split Frustum
// f : 0 ~ 1
void cFrustum::Split2(cCamera &cam, const float f1, const float f2
	, cFrustum &out1, cFrustum &out2)
{
	const float oldNearPlane = cam.m_nearPlane;
	const float oldFarPlane = cam.m_farPlane;
	const float far1 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f1);
	const float far2 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f2); 

	cam.ReCalcProjection(cam.m_nearPlane, far1);
	out1.SetFrustum(cam.GetViewProjectionMatrix());
	
	cam.ReCalcProjection(far1, far2);
	out2.SetFrustum(cam.GetViewProjectionMatrix());

	// recovery
	cam.ReCalcProjection(oldNearPlane, oldFarPlane);
}


// Split Frustum
// f : 0 ~ 1
void cFrustum::Split3(cCamera &cam, const float f1, const float f2, const float f3
	, cFrustum &out1, cFrustum &out2, cFrustum &out3)
{
	const float oldNearPlane = cam.m_nearPlane;
	const float oldFarPlane = cam.m_farPlane;
	const float far1 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f1);
	const float far2 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f2);
	const float far3 = common::lerp(cam.m_nearPlane, cam.m_farPlane, f3);

	cam.ReCalcProjection(cam.m_nearPlane, far1);
	out1.SetFrustum(cam.GetViewProjectionMatrix());

	cam.ReCalcProjection(far1, far2);
	out2.SetFrustum(cam.GetViewProjectionMatrix());

	cam.ReCalcProjection(far2, far3);
	out3.SetFrustum(cam.GetViewProjectionMatrix());

	// recovery
	cam.ReCalcProjection(oldNearPlane, oldFarPlane);
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
	Vector3 vertices[8] = {
		Vector3(-1,1,0), Vector3(1,1,0), Vector3(-1,-1,0), Vector3(1,-1,0),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};
	Matrix44 matInv = m_viewProj.Inverse();
	for (int i = 0; i < 8; ++i)
		vertices[i] *= matInv;

	// Far Plane
	Vector3 p0, p1;
	if (plane.LineCross(vertices[0], vertices[4], &p0) == 0)
		p0 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	if (plane.LineCross(vertices[1], vertices[5], &p1) == 0)
		p1 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 p4 = plane.Pick(vertices[4], (vertices[6] - vertices[4]).Normal());
	Vector3 p5 = plane.Pick(vertices[5], (vertices[7] - vertices[5]).Normal());

	// Near Plane
	Vector3 p2, p3;
	if (plane.LineCross(vertices[3], vertices[7], &p2) == 0)
		p2 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	if (plane.LineCross(vertices[2], vertices[6], &p3) == 0)
		p3 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 p6 = plane.Pick(vertices[1], (vertices[3] - vertices[1]).Normal());
	Vector3 p7 = plane.Pick(vertices[0], (vertices[2] - vertices[0]).Normal());

	outVertices[0] = (m_pos.LengthRoughly(p0) < m_pos.LengthRoughly(p4)) ? p0 : p4;
	outVertices[1] = (m_pos.LengthRoughly(p1) < m_pos.LengthRoughly(p5)) ? p1 : p5;
	outVertices[2] = (m_pos.LengthRoughly(p2) < m_pos.LengthRoughly(p6)) ? p2 : p6;
	outVertices[3] = (m_pos.LengthRoughly(p3) < m_pos.LengthRoughly(p7)) ? p3 : p7;
}
