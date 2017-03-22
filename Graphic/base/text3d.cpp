
#include "stdafx.h"
#include "text3d.h"


using namespace graphic;

cText3d::cText3d()
{
}

cText3d::~cText3d()
{
}


bool cText3d::Create(cRenderer &renderer, cFontGdi *font, const BILLBOARD_TYPE::TYPE type, 
	const int width, const int height, const int textWidth, const int textHeight)
	//textWidth = 128
	//textHeight = 64
{
	if (!m_texture.Create(renderer, textWidth, textHeight, D3DFMT_A8R8G8B8))
		return false;

	if (!m_quad.Create(renderer, type, (float)width, (float)height, Vector3(0, 0, 0)))
		return false;

	m_quad.m_texture = &m_texture;
	m_font = font;
	return true;
}


bool cText3d::SetText(const Matrix44 &tm, const string &text, const DWORD color)
{
	RETV(!m_font, false);

	m_quad.m_tm = tm;
	m_color = color;
	FillTexture(color);
	m_texture.TextOut(*m_font, text, 0, 0, color);

	return true;
}


bool cText3d::SetTextRect(const Matrix44 &tm, const string &text, const DWORD color, const sRect &rect)
{
	RETV(!m_font, false);

	m_quad.m_tm = tm;
	m_color = color;
	FillTexture(color);
	m_texture.DrawText(*m_font, text, rect, color);

	return true;
}


// Set Texture Background Color
void cText3d::FillTexture(const DWORD color)
{
	D3DLOCKED_RECT lockRect;
	m_texture.Lock(lockRect);
	if (lockRect.pBits)
	{
		DWORD *p = (DWORD*)lockRect.pBits;
		for (u_int i = 0; i < m_texture.m_imageInfo.Height; ++i)
			for (int k = 0; k < lockRect.Pitch; k += 4)
				*p++ = color;
		m_texture.Unlock();
	}
}


void cText3d::Render(cRenderer &renderer)
{
	// AlphaBlending
	// src, dest inverse alpha
	//renderer.SetCullMode(D3DCULL_NONE);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, m_color);
	m_quad.RenderFactor(renderer);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//renderer.SetCullMode(D3DCULL_CCW);
}
