
#include "stdafx.h"
#include "dbgquad.h"

using namespace graphic;


cDbgQuad::cDbgQuad()
{
}

cDbgQuad::~cDbgQuad()
{
}


bool cDbgQuad::Create(cRenderer &renderer)
{
	for (int i = 0; i < 4; ++i)
		m_lines[i].Create(renderer);

	return true;
}


//   0 -------------- 1
//   |                     |
//   |                     |
//   |                     |
//   3 -------------- 2
//    Vertex Index
void cDbgQuad::SetQuad(const Vector3 vertices[4]
	, const float width //= 1.f
)
{
	for (int i = 0; i < 4; ++i)
	{
		m_lines[i].SetLine(vertices[i % 4], vertices[(i + 1) % 4], width);
		m_pos[i] = vertices[i];
	}
}


void cDbgQuad::SetColor(const cColor &color)
{
	for (int i = 0; i < 4; ++i)
		m_lines[i].SetColor(color);
}


void cDbgQuad::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	for (int i = 0; i < 4; ++i)
		m_lines[i].Render(renderer, tm);
}
