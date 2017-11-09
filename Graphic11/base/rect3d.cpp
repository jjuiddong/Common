
#include "stdafx.h"
#include "rect3d.h"

using namespace graphic;


cRect3D::cRect3D()
	: cNode(common::GenerateId(), "rect3d", eNodeType::MODEL)
{
}

cRect3D::~cRect3D()
{
}


bool cRect3D::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	const float width = 0.1f;

	renderer.m_line.SetColor(cColor::BLACK);
	renderer.m_line.SetLine(m_pos[0], m_pos[1], width);
	renderer.m_line.Render(renderer, parentTm, flags);

	renderer.m_line.SetLine(m_pos[1], m_pos[3], width);
	renderer.m_line.Render(renderer, parentTm, flags);

	renderer.m_line.SetLine(m_pos[3], m_pos[2], width);
	renderer.m_line.Render(renderer, parentTm, flags);

	renderer.m_line.SetLine(m_pos[2], m_pos[0], width);
	renderer.m_line.Render(renderer, parentTm, flags);

	return true;
}


void cRect3D::SetRect(const Vector3 pos[4])
{
	for (int i = 0; i < 4; ++i)
		m_pos[i] = pos[i];
}
