
#include "stdafx.h"
#include "rect2d.h"

using namespace graphic;


cRect2D::cRect2D()
	: cNode(common::GenerateId(), "rect2d", eNodeType::MODEL)
{
}


void cRect2D::SetRect(const Vector2 &leftTop, const Vector2 &rightBottom, const float width)
{
	const Vector2 size = rightBottom - leftTop;
	m_transform.pos = Vector3(leftTop.x, leftTop.y, 0);
	m_transform.scale = Vector3(abs(size.x), abs(size.y), width);
}


void cRect2D::SetColor(const cColor &color)
{
	m_color = color;
}


bool cRect2D::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	const Vector2 leftTop = Vector2(m_transform.pos.x, m_transform.pos.y);

	Vector2 pos[4]; // lefttop, righttop, leftbottom, rightbottom
	pos[0] = leftTop;
	pos[1] = leftTop + Vector2(m_transform.scale.x, 0);
	pos[2] = leftTop + Vector2(0, m_transform.scale.y);
	pos[3] = leftTop + Vector2(m_transform.scale.x, m_transform.scale.y);

	renderer.m_line2D.SetColor(m_color);
	renderer.m_line2D.SetLine(pos[0], pos[1], m_transform.scale.z);
	renderer.m_line2D.Render(renderer, parentTm, flags);

	renderer.m_line2D.SetLine(pos[1], pos[3], m_transform.scale.z);
	renderer.m_line2D.Render(renderer, parentTm, flags);

	renderer.m_line2D.SetLine(pos[0], pos[2], m_transform.scale.z);
	renderer.m_line2D.Render(renderer, parentTm, flags);

	renderer.m_line2D.SetLine(pos[2], pos[3], m_transform.scale.z);
	renderer.m_line2D.Render(renderer, parentTm, flags);

	return true;
}
