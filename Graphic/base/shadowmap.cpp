
#include "stdafx.h"
#include "shadowmap.h"


using namespace graphic;

cShadowMap::cShadowMap()
{
}

cShadowMap::~cShadowMap()
{
}


bool cShadowMap::Create(cRenderer &renderer, const int textureWidth, const int textureHeight)
{
	return m_surface.Create(renderer, textureWidth, textureHeight, 1
		, D3DFMT_R32F
		, true
		, D3DFMT_D24X8
		, false
	, NULL, 0.1f, 100.f);
}


void cShadowMap::Bind(cShader &shader, const string &key)
{
	m_surface.Bind(shader, key);
}


void cShadowMap::Begin(cRenderer &renderer)
{
	float maxFlt = FLT_MAX;
	m_surface.Begin( renderer);
	renderer.GetDevice()->Clear(0, NULL
		, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
		//, *(DWORD*)&maxFlt
		, 0xffffffff
		, 1.0f, 0L);
}


void cShadowMap::End(cRenderer &renderer)
{
	m_surface.End(renderer);
}


bool cShadowMap::IsLoaded() const
{
	return m_surface.IsLoaded();
}


void cShadowMap::LostDevice()
{
	m_surface.LostDevice();
}


void cShadowMap::ResetDevice(graphic::cRenderer &renderer)
{
	m_surface.ResetDevice(renderer);
}


// 디버깅용 함수.
void cShadowMap::Render(cRenderer &renderer
	, const int index //=1
)
{
	m_surface.Render(renderer, index);
}
