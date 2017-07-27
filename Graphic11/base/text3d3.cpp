
#include "stdafx.h"
#include "text3d3.h"


using namespace graphic;

cText3d3::cText3d3()
	: cNode2(common::GenerateId(), "text", eNodeType::TEXT)
{
}

cText3d3::~cText3d3()
{
}


bool cText3d3::Create(cRenderer &renderer, cFontGdi *font, const BILLBOARD_TYPE::TYPE type,
	const int width, const int height
	, const int textWidth // = 256
	, const int textHeight // = 32
)
{
	if (!m_texture.Create(renderer, textWidth, textHeight, D3DFMT_A8R8G8B8))
		return false;

	if (!m_quad.Create(renderer, type, (float)width, (float)height, Vector3(0, 0, 0)))
		return false;

	m_quad.m_texture = &m_texture;
	m_font = font;
	m_boundingBox.SetBoundingBox(Vector3(-width / 2, -height / 2, 0),
		Vector3(width / 2, height / 2, 0));
	CalcBoundingSphere();

	return true;
}


bool cText3d3::SetText(const Matrix44 &tm, const Str128 &text, const cColor &color)
{
	RETV(!m_font, false);

	m_text = text;
	m_quad.m_tm = tm;
	m_color = color;
	FillTexture(color);
	m_texture.TextOut(*m_font, text, 0, 0, color.m_color);

	return true;
}


bool cText3d3::SetTextRect(
	//const Matrix44 &tm
	const Transform &tm
	, const Str128 &text
	, const cColor &color
	, const BILLBOARD_TYPE::TYPE type
	, const sRecti &rect)
{
	RETV(!m_font, false);

	m_transform = tm;
	m_quad.m_type = type;
	m_quad.m_transform = tm;
	// todo: sorting bug
	//m_boundingBox.SetBoundingBox(tm.pos-Vector3(1,1,1)*0.5f, tm.pos + Vector3(1, 1, 1)*0.5f); 

	if ((m_color != color) || (m_text != text))
	{
		m_color = color;
		FillTexture(color);
		m_texture.DrawText(*m_font, text, rect, color.m_color);
		m_text = text;
	}

	return true;
}


bool cText3d3::SetTextRect2(
	cRenderer &renderer
	, const Transform &tm
	, const Str128 &text
	, const cColor &color, const cColor &outlineColor
	, const BILLBOARD_TYPE::TYPE type
	)
{
	m_transform = tm;
	m_quad.m_type = type;
	m_quad.m_transform.pos = tm.pos;
	m_quad.m_transform.rot = tm.rot;

	//const Vector3 dim = m_boundingBox.GetDimension();
	//m_boundingBox.SetBoundingBox(Vector3(-dim.x / 2, -dim.y/ 2, 0),
	//	Vector3(dim.x / 2, dim.y/ 2, 0));
	//CalcBoundingSphere();

	//m_quad.m_transform = tm;
	// todo: sorting bug
	//m_boundingBox.SetBoundingBox(tm.pos-Vector3(1,1,1)*0.5f, tm.pos + Vector3(1, 1, 1)*0.5f); 

	if ((m_color != color) || (m_text != text))
	{
		m_color = color;

		Vector2 textSize;
		if (m_texture.DrawText2(renderer, text, color, outlineColor, textSize))
		{
			const float u = textSize.x / (float)m_texture.m_imageInfo.Width;
			m_quad.SetUV(Vector2(0, 0), Vector2(u, 0), Vector2(0, 1), Vector2(u, 1));
			m_quad.m_transform.scale = Vector3(u * tm.scale.x, tm.scale.y, tm.scale.z);
		}

		m_text = text;
	}

	return true;
}


// Set Texture Background Color
void cText3d3::FillTexture(const cColor &color)
{
	D3DLOCKED_RECT lockRect;
	m_texture.Lock(lockRect);
	if (lockRect.pBits)
	{
		DWORD *p = (DWORD*)lockRect.pBits;
		for (u_int i = 0; i < m_texture.m_imageInfo.Height; ++i)
			for (int k = 0; k < lockRect.Pitch; k += 4)
				*p++ = color.m_color;
		m_texture.Unlock();
	}
}


bool cText3d3::RenderOld(cRenderer &renderer
	, const Matrix44 &parentTm //= Matrix44::Identity
	, const int flags //= 1
)
{
	// AlphaBlending
	// src, dest inverse alpha
	renderer.SetCullMode(D3DCULL_NONE);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, m_color);
	m_quad.RenderFactor(renderer);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	renderer.SetCullMode(D3DCULL_CCW);
	return true;
}


bool cText3d3::Render(cRenderer &renderer
	, const Matrix44 &parentTm //= Matrix44::Identity
	, const int flags //= 1
)
{
	// AlphaBlending
	// src, dest inverse alpha
	renderer.SetCullMode(D3DCULL_NONE);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	renderer.GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	renderer.GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_quad.Render(renderer);
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	renderer.SetCullMode(D3DCULL_CCW);

	return true;
}
