
#include "stdafx.h"
#include "color.h"

using namespace graphic;


cColor::cColor()
	: m_color(0)
{
}

cColor::~cColor()
{
}


// r, g, b : 0 ~ 1 
void cColor::SetColor(const float r, const float g, const float b
	, const float a //= 1.f
)
{
	m_color = D3DCOLOR_COLORVALUE(r, g, b, a);

}


// r, g, b : 0 ~ 255
void cColor::SetColor(const BYTE r, const BYTE g, const BYTE b
	, const BYTE a //= 255
)
{
	m_color = D3DCOLOR_ARGB(a, r, g, b);
}


// x-r, y-g, z-b : 0 ~ 1 
void cColor::SetColor(const Vector3 &color)
{
	SetColor(color.x, color.y, color.z);
}

// x-r, y-g, z-b, w-a : 0 ~ 1 
void cColor::SetColor(const Vector4 &color)
{
	SetColor(color.x, color.y, color.z, color.w);
}

