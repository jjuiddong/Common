
#include "stdafx.h"
#include "bezierline.h"

using namespace graphic;


cBezierLine::cBezierLine()
	: cNode(common::GenerateId(), "BezierLine")
	, m_slice(10)
{
}

cBezierLine::~cBezierLine()
{
}


bool cBezierLine::Create(cRenderer &renderer, const uint slice, const cColor &color)
{
	m_slice = slice;
	m_color = color;

	m_line.Create(renderer, Vector3(0,0,0), Vector3(1,1,1), 1.f
		, eVertexType::POSITION | eVertexType::COLOR, color);
	m_line.m_transform = Transform(); // initizlie transform
	m_line.m_transform.pos.y = 0.f;

	return true;
}


bool cBezierLine::Update(cRenderer &renderer, const float deltaSeconds)
{
	return true;
}


bool cBezierLine::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(m_pts.empty(), true);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);

	const bool result = m_line.RenderInstancing(renderer
		,(int)m_pts.size(), &m_pts[0], parentTm, flags);

	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	return result;
}


void cBezierLine::SetLine(const Vector3 &p0, const Vector3 &p1,
	const Vector3 &p2, const Vector3 &p3, const float width)
{
	RET(m_slice <= 4);

	m_points[0] = p0;
	m_points[1] = p1;
	m_points[2] = p2;
	m_points[3] = p3;

	vector<Vector3> out;
	out.reserve(m_slice + 1);
	float inc = 1.f / (float)m_slice;
	float a = 0.f;
	for (uint i = 0; i < m_slice+1; ++i)
	{
		Vector3 pos;
		common::bezier(pos, m_points, a);
		out.push_back(pos);
		a += inc;
	}

	const Matrix44 tm = m_transform.GetMatrix();
	
	m_pts.clear();
	m_pts.reserve(m_slice + 1);
	for (uint i = 0; i < out.size() - 1; ++i)
	{
		const Vector3 p0 = out[i];
		const Vector3 p1 = out[i + 1];

		// p0에서 p1으로 향하는 Cube Transform 생성
		// anchor는 Cube의 중심
		Vector3 v = p1 - p0;
		const float len = v.Length();
		v.Normalize();
		
		Transform transform;
		transform.pos = (p0 + p1) / 2.f;
		transform.scale = Vector3(width, width, len / 2.f);
		Quaternion q(Vector3(0, 0, 1), v);
		transform.rot = q;

		m_pts.push_back(transform.GetMatrix() * tm);
	}
}


void cBezierLine::SetLine(const Vector3 p[4], const float width)
{
	SetLine(p[0], p[1], p[2], p[3], width);
}


void cBezierLine::SetLine(const Vector3 &p0, const Vector3 &p1
	, const float height, const float width)
{
	Vector3 p2, p3;
	common::lerp(p2, p0, p1, 0.25f);
	common::lerp(p3, p0, p1, 0.65f);

	const float len = p0.Distance(p1);
	p2.z += min(1.f, (len * 0.3f));
	p3.z -= (len * 0.2f);

	SetLine(p0, p2, p3, p1, width);
}
