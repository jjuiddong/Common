
#include "stdafx.h"
#include "dbgquad2.h"

using namespace graphic;


cDbgQuad2::cDbgQuad2()
{
}

cDbgQuad2::~cDbgQuad2()
{
}


bool cDbgQuad2::Create(cRenderer &renderer)
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
void cDbgQuad2::SetQuad(Vector3 vertices[4]
	, const float width //= 1.f
)
{
	for (int i = 0; i < 4; ++i)
	{
		m_lines[i].SetLine(vertices[i % 4], vertices[(i + 1) % 4], width);
		m_pos[i] = vertices[i];
	}
}


void cDbgQuad2::SetColor(const DWORD color)
{
	for (int i = 0; i < 4; ++i)
		m_lines[i].SetColor(color);
}


void cDbgQuad2::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	for (int i = 0; i < 4; ++i)
		m_lines[i].Render(renderer);
}
