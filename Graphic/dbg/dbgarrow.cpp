
#include "stdafx.h"
#include "dbgarrow.h"

using namespace graphic;


cDbgArrow::cDbgArrow()
{
}

cDbgArrow::~cDbgArrow()
{
}


bool cDbgArrow::Create(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1
	, const float size //= 1.f
)
{
	m_head.Create(renderer, size, 8, 8);
	m_head.m_tm.SetPosition(p1);
	m_body.SetLine(renderer, p0, p1, size * 0.5f, D3DXCOLOR(1,1,1,1));
	return true;
}


void cDbgArrow::Render(cRenderer &renderer)
{
	m_head.Render(renderer);
	m_body.Render(renderer);
}


void cDbgArrow::SetDirection(const Vector3 &p0, const Vector3 &p1
	, const float size //= 1.f
)
{
	m_head.m_tm.SetPosition(p1);
	m_body.SetLine(p0, p1, size * 0.5f, D3DXCOLOR(1, 1, 1, 1));
}
