
#include "stdafx.h"
#include "text3d2.h"


using namespace graphic;

cText3d2::cText3d2()
{
}

cText3d2::~cText3d2()
{
	m_texture.m_texture = NULL;
}


bool cText3d2::Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type,
	const int width, const int height
	, const int textWidth //= 128
	, const int textHeight //= 64
)
{
	if (!m_surface.Create(renderer, textWidth, textHeight, 1, D3DFMT_A8R8G8B8))
	//if (!m_surface.CreateRenderTarget(renderer, textWidth, textHeight))
		return false;

	if (!m_quad.Create(renderer, type, (float)width, (float)height, Vector3(0, 0, 0)))
		return false;

	m_texture.m_texture = m_surface.GetTexture();
	m_quad.m_texture = &m_texture;

	m_text.Create(renderer);

	return true;
}


bool cText3d2::SetText(const Matrix44 &tm, const Str128 &text, const DWORD color)
{
	return true;
}


bool cText3d2::SetTextRect(cRenderer &renderer, const Matrix44 &tm, const Str128 &text, const DWORD color)// , const sRecti &rect)
{
	m_quad.m_tm = tm;
	m_color = color;

	m_text.SetText(text);

	m_surface.Begin(renderer);
	if (SUCCEEDED(renderer.GetDevice()->Clear(
		0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(0, 50, 50, 50),
		1.0f, 0)))
	{
		const Vector2i offset[] = {
			{1,0}, {2,0}, {2,1}, { 2,2 },
			{1,2}, { 0,2 }, {0,1}, { 0,0}
		};

		m_text.SetColor(D3DCOLOR_XRGB(0, 0, 0));
		for (int i = 0; i < sizeof(offset) / sizeof(*offset); ++i)
		{
			m_text.SetPos(offset[i].x, offset[i].y);
			m_text.Render();
		}

		m_text.SetColor(color);
		m_text.SetPos(1, 1);
		m_text.Render();
	}
	m_surface.End(renderer);

	return true;
}


void cText3d2::Render(cRenderer &renderer)
{
	// AlphaBlending
	// src, dest inverse alpha
	//renderer.SetCullMode(D3DCULL_NONE);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, m_color);
	m_quad.Render(renderer);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//renderer.SetCullMode(D3DCULL_CCW);
}


void cText3d2::LostDevice()
{
	m_texture.m_texture = NULL;
	m_text.LostDevice();
	m_surface.LostDevice();
}


void cText3d2::ResetDevice(cRenderer &renderer)
{
	m_text.ResetDevice(renderer);
	m_surface.ResetDevice(renderer);
	m_texture.m_texture = m_surface.m_texture;

	SetTextRect(renderer, m_quad.m_tm, m_text.m_text, m_color);
}
