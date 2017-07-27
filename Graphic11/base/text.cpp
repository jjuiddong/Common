
#include "stdafx.h"
#include "text.h"


using namespace graphic;


cText::cText()
	: m_font(NULL)
,	m_color(D3DXCOLOR(1,1,1,1))
,	m_rect(0,0,100,100)
,	m_sprite(NULL)
{
}

cText::cText(cRenderer &renderer, const Str128 &text, const int x, const int y,
	const DWORD color, const int fontSize, const bool isBold, const Str32 &fontName)
	// fontSize=18, isBold=true, fontName=굴림
	: m_text(text)
,	m_font(NULL)
,	m_rect(x, y, 100, 100)
,	m_color(color)
{
	Create(renderer, fontSize, isBold, fontName);
}


cText::~cText()
{
	Clear();
}


// 텍스트 생성.
bool cText::Create(cRenderer &renderer, const int fontSize, const bool isBold, const Str32 &fontName, cSprite *sprite)
	// fontSize=18, isBold=true, fontName=굴림
{
	Clear();

	const HRESULT hr = D3DXCreateFontA(renderer.GetDevice(), fontSize, 0, 
		isBold?  FW_BOLD : FW_NORMAL, 1, FALSE, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName.c_str(), 
		&m_font );

	if (FAILED(hr))
		return false;

	m_sprite = sprite;

	return true;
}


// 텍스트 출력.
void cText::Render()
{
	RET(!m_font);

	if (m_sprite)
	{
		m_sprite->Begin();
		m_font->DrawTextA(m_sprite->m_p, m_text.c_str(), -1, (RECT*)&m_rect, DT_NOCLIP, m_color);
		m_sprite->End();
	}
	else
	{
		m_font->DrawTextA( NULL, m_text.c_str(), -1, (RECT*)&m_rect, DT_NOCLIP, m_color);
	}
}


void cText::Clear()
{
	SAFE_RELEASE(m_font);
	m_sprite = NULL;
}


void cText::LostDevice()
{
	if (m_font)
		m_font->OnLostDevice();
}


void cText::ResetDevice(cRenderer &renderer)
{
	if (m_font)
		m_font->OnResetDevice();
}
