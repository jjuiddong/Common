
#include "stdafx.h"
#include "sprite.h"

using namespace graphic;

cSprite cSprite::NullSprite;


cSprite::cSprite()
	: m_p(NULL)
{
}

cSprite::~cSprite()
{
	Clear();
}


bool cSprite::Create(cRenderer &renderer)
{
	Clear();

	const HRESULT hr = D3DXCreateSprite(renderer.GetDevice(), &m_p);
	if (FAILED(hr))
		return false;

	return true;
}


void cSprite::Render(cTexture &texture, const sRecti &rect, const Vector3 &center, const D3DCOLOR color, 
	const Matrix44 &tm)
// tm = Matrix44::Identity
{
	RET(!m_p);
	
	m_p->SetTransform((D3DXMATRIX*)&tm);
	m_p->Draw(texture.GetTexture(), (RECT*)&rect, (D3DXVECTOR3*)&center, NULL, color);
}


void cSprite::Begin()
{
	RET(!m_p);
	m_p->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
}


void cSprite::End()
{
	RET(!m_p);
	m_p->End();
}


void cSprite::Clear()
{
	SAFE_RELEASE(m_p);
}

void cSprite::LostDevice()
{
	if (m_p)
		m_p->OnLostDevice();
}


void cSprite::ResetDevice(cRenderer &renderer)
{
	if (m_p)
		m_p->OnResetDevice();
}
