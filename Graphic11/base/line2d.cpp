
#include "stdafx.h"
#include "line2d.h"

using namespace graphic;


cLine2d::cLine2d(const float width) // width=1
	: m_dxLine(NULL)
	, m_width(width)
	, m_color(D3DCOLOR_XRGB(0, 0, 255))
{
	m_vList[0] = Vector2(0, 0);
	m_vList[1] = Vector2(0, 0);
}

cLine2d::~cLine2d()
{
	if (m_dxLine)
	{
		m_dxLine->Release();
		m_dxLine = NULL;
	}
}


void cLine2d::Render(cRenderer &renderer)
{
	if (!m_dxLine)
	{
		D3DXCreateLine(renderer.GetDevice(), &m_dxLine); // Line 持失
	}

	m_dxLine->SetWidth(m_width);

	m_dxLine->Begin();
	m_dxLine->Draw((D3DXVECTOR2*)m_vList, 2, m_color);
	m_dxLine->End();
}


void cLine2d::Render(cRenderer &renderer, const Vector2 *vList, const int vListCount)
{
	if (!m_dxLine)
	{
		D3DXCreateLine(renderer.GetDevice(), &m_dxLine); // Line 持失
	}

	m_dxLine->SetWidth(m_width);

	m_dxLine->Begin();
	m_dxLine->Draw((D3DXVECTOR2*)vList, vListCount, m_color);
	m_dxLine->End();
}


void cLine2d::SetLine(const Vector2 &p0, const Vector2 &p1)
{
	m_vList[0] = p0;
	m_vList[1] = p1;
}

