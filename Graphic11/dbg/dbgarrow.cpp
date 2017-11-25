
#include "stdafx.h"
#include "dbgarrow.h"

using namespace graphic;


cDbgArrow::cDbgArrow()
	: m_isSolid(false)
	, m_color(cColor::WHITE)
{
}

cDbgArrow::~cDbgArrow()
{
}


bool cDbgArrow::Create(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1
	, const float size //= 1.f
	, const bool isSolid //= false
)
{
	m_head.Create(renderer, size, size, p0, eVertexType::POSITION);
	m_head.SetDirection(p0, p1, p1, size*1.75f, 0.75f);

	m_body.Create(renderer, p0, p1, size * 0.4f, eVertexType::POSITION);

	m_isSolid = isSolid;
	return true;
}


void cDbgArrow::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	m_head.m_color = m_color;
	m_body.m_color = m_color;

	if (m_isSolid)
	{
		m_head.Render(renderer, tm);
		m_body.Render(renderer, tm);
	}
	else
	{
		CommonStates states(renderer.GetDevice());
		renderer.GetDevContext()->RSSetState(states.Wireframe());
		m_head.Render(renderer, tm);
		m_body.Render(renderer, tm);
		renderer.GetDevContext()->RSSetState(NULL);
	}
}


void cDbgArrow::SetDirection(const Vector3 &p0, const Vector3 &p1
	, const float size //= 1.f
)
{
	m_head.SetDirection(p0, p1, p1, size*1.75f, 0.75f);
	m_body.SetLine(p0, p1, size * 0.4f);
	m_transform.pos = (p0 + p1) * 0.5f;
}


bool cDbgArrow::Picking(const Ray &ray
	, const XMMATRIX &parentTm //= XMIdentity
	, const bool isSpherePicking //= true
	, OUT float *dist //= NULL
)
{
	float out[2] = { FLT_MAX, FLT_MAX };
	const bool r1 = m_head.Picking(ray, eNodeType::MODEL, parentTm, isSpherePicking, &out[0]);
	const bool r2 = m_body.Picking(ray, eNodeType::MODEL, parentTm, isSpherePicking, &out[1]);

	if (dist)
	{
		if (r1 && !r2)
			*dist = out[0];
		else if (!r1 && r2)
			*dist = out[1];
		else if (r1 && r2)
			*dist = max(out[0], out[1]);
	}

	return r1 || r2;
}
