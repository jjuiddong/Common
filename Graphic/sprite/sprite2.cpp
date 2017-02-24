
#include "stdafx.h"
#include "sprite2.h"

using namespace graphic;


cSprite2::cSprite2(cSprite &sprite, const int id, const string &name) :
	cNode(id, name)
	, m_sprite(sprite)
	, m_texture(NULL)
	, m_pos(Vector3(0, 0, 0))
	, m_scale(Vector3(1, 1, 1))
	, m_color(D3DCOLOR_XRGB(255, 255, 255))
{
	::SetRect(&m_rect, 0, 0, 0, 0);
}

cSprite2::~cSprite2()
{
}


void cSprite2::SetTexture(cRenderer &renderer, const string &fileName)
{
	m_texture = cResourceManager::Get()->LoadTexture(renderer, fileName, false);
	if (m_texture)
	{
		//if (::IsRectEmpty(&m_rect))
		{
			m_rect.SetWidth(m_texture->GetImageInfo().Width);
			m_rect.SetHeight(m_texture->GetImageInfo().Height);
		}
	}
}


void cSprite2::SetCenter(const Vector3 &center) // x,y = 0~1
{
	m_center = Vector3(m_rect.Width()*center.x, m_rect.Height()*center.y, 0);
}


void cSprite2::Render(cRenderer &renderer, const Matrix44 &parentTm)
{
	RET(!m_sprite.m_p);

	Matrix44 S, T;
	S.SetScale(m_scale);
	T.SetTranslate(m_pos);

	m_accTM = T * parentTm;
	Matrix44 tm = S * m_accTM;

	if (m_texture)
	{
		m_sprite.Begin();
		renderer.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		renderer.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		m_sprite.Render(*m_texture, m_rect, m_center, m_color, tm);
		m_sprite.End();
	}

	// 자식노드들의 Render를 호출한다.
	cNode::Render(renderer, m_accTM);
}


// screen pixel 좌표 pos 값이 스프라이트 안에 있다면 true를 리턴한다.
bool cSprite2::IsContain(const Vector2 &pos)
{
	Vector3 leftTop(0, 0, 0);
	Vector3 rightBottom = Vector3((float)m_rect.Width(), (float)m_rect.Height(), 0);

	Matrix44 S;
	S.SetScale(m_scale);
	Matrix44 C;
	C.SetTranslate(-m_center);
	Matrix44 tm = S * C * m_accTM;
	leftTop *= tm;
	rightBottom *= tm;

	return ((leftTop.x <= pos.x) &&
		(leftTop.y <= pos.y) &&
		(rightBottom.x >= pos.x) &&
		(rightBottom.y >= pos.y));
}


void cSprite2::Clear()
{
	m_texture = NULL;
}


void cSprite2::LostDevice()
{

}

void cSprite2::ResetDevice(cRenderer &renderer)
{

}
