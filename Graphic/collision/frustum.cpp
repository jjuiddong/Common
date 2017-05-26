
#include "stdafx.h"
#include "frustum.h"

using namespace graphic;

// 프러스텀에 정확하게 포함되지 않더라도, 약간의 여분을 주어서 프러스텀에 포함시키기 위한 값
#define PLANE_EPSILON		5.0f


cFrustum::cFrustum()
:	m_fullCheck(false)
,	m_plane(6) // 절두체 평면 6개
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

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 정육면체의 minimum pos 와 maximum pos 로 절두체를 만든다.
//-----------------------------------------------------------------------------//
bool cFrustum::SetFrustum(const Vector3 &_min, const Vector3 &_max)
{
	//sVertexDiffuse *vertices = (sVertexDiffuse*)m_vtxBuff.Lock();
	//RETV(!vertices, false);

	//m_fullCheck = true;

	//m_pos = (_min + _max) / 2.0f;

	//// 얻어진 월드좌표로 프러스텀 평면을 만든다
	//// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
	//m_plane[0].Init( vertices[ 0].p, vertices[ 1].p, vertices[ 2].p );	// 근 평면(near)
	//m_plane[1].Init( vertices[ 0].p, vertices[ 4].p, vertices[ 1].p );	// 윗 평면(top)
	//m_plane[2].Init( vertices[ 2].p, vertices[ 3].p, vertices[ 6].p );	// 아래 평면(bottom)

	//m_plane[3].Init( vertices[ 4].p, vertices[ 6].p, vertices[ 7].p );	// 원 평면(far)
	//m_plane[4].Init( vertices[ 0].p, vertices[ 2].p, vertices[ 6].p );	// 좌 평면(left)
	//m_plane[5].Init( vertices[ 1].p, vertices[ 5].p, vertices[ 7].p );	// 우 평면(right)

	//m_vtxBuff.Unlock();
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
		if (dist > PLANE_EPSILON) 
			return false;
	}

	return true;
}


//-----------------------------------------------------------------------------//
// 중심(point)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
// TRUE를 반환, 아니면 FALSE를 반환한다.
//-----------------------------------------------------------------------------//
bool cFrustum::IsInSphere( const Vector3 &point, float radius ) const
{
	for (int i=0; i < 6; ++i)
	{
		// m_fullCheck 가 false 라면 near, top, bottom  평면 체크는 제외 된다.
		if (!m_fullCheck && (i < 3))
			continue;

		// 평면과 중심점의 거리가 반지름보다 크면 프러스텀에 없음
		const float dist = m_plane[ i].Distance( point );
		if (dist > (radius+PLANE_EPSILON)) 
			return false;
	}

	return true;
}


bool cFrustum::IsInSphere(const cBoundingSphere &sphere) const
{
	return IsInSphere(sphere.m_pos, sphere.m_radius);
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
