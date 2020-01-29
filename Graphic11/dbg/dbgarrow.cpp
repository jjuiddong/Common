
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
	, const float width //= 1.f
	, const bool isSolid //= false
	, const float arrowRatio //= 0.75f
)
{
	m_head.Create(renderer, width, width, p0, eVertexType::POSITION);
	m_head.SetDirection(p0, p1, p1, width*1.75f, arrowRatio);

	cBoundingBox bbox;
	bbox.SetBoundingBox(Transform(p1, Vector3::Ones*width*1.75f));
	m_headCube.Create(renderer, bbox, eVertexType::POSITION);

	m_body.Create(renderer, p0, p1, width * 0.4f, eVertexType::POSITION);

	m_isSolid = isSolid;
	return true;
}


void cDbgArrow::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const bool showCubeHead //= false
)
{
	m_head.m_color = m_color;
	m_headCube.m_color = m_color;
	m_body.m_color = m_color;

	if (m_isSolid)
	{
		CommonStates states(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);

		if (showCubeHead)
			m_headCube.Render(renderer, tm);
		else
			m_head.Render(renderer, tm);
		m_body.Render(renderer, tm);

		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else
	{
		CommonStates states(renderer.GetDevice());
		ID3D11RasterizerState *oldState = NULL;
		renderer.GetDevContext()->RSGetState(&oldState);
		renderer.GetDevContext()->RSSetState(states.Wireframe());
		if (showCubeHead)
			m_headCube.Render(renderer, tm);
		else
			m_head.Render(renderer, tm);
		m_body.Render(renderer, tm);
		renderer.GetDevContext()->RSSetState(oldState);
	}
}


void cDbgArrow::SetDirection(const Vector3 &p0, const Vector3 &p1
	, const float width //= 1.f
	, const float arrowRatio //= 0.75f
)
{
	m_head.SetDirection(p0, p1, p1, width*1.75f, arrowRatio);
	m_body.SetLine(p0, p1, width * 0.4f);

	cBoundingBox bbox;
	bbox.SetBoundingBox(Transform(p1, Vector3::Ones*width*1.5f));
	m_headCube.SetCube(bbox);

	m_transform.pos = (p0 + p1) * 0.5f;
}


bool cDbgArrow::Picking(const Ray &ray
	, const XMMATRIX &parentTm //= XMIdentity
	, const bool isSpherePicking //= true
	, OUT float *dist //= NULL
)
{
	float out[2] = { FLT_MAX, FLT_MAX };
	const bool r1 = m_head.Picking(ray, eNodeType::MODEL, parentTm, isSpherePicking, &out[0]) ? true : false;
	const bool r2 = m_body.Picking(ray, eNodeType::MODEL, parentTm, isSpherePicking, &out[1]) ? true : false;

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


void cDbgArrow::SetColor(const cColor &color)
{
	m_color = color;
}


void cDbgArrow::SetTechnique(const char *techniqName)
{
	m_head.SetTechnique(techniqName);
	m_body.SetTechnique(techniqName);
}
