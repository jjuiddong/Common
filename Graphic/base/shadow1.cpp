
#include "stdafx.h"
#include "shadow1.h"

using namespace graphic;


cShadow1::cShadow1()
{
}

cShadow1::~cShadow1()
{
}


// 그림자 클래스 생성.
bool cShadow1::Create(cRenderer &renderer, const int textureWidth, const int textureHeight)
{
	return m_surface.Create(renderer, textureWidth, textureHeight, 1,
		D3DFMT_X8R8G8B8, true, D3DFMT_D24X8, true);
}


// 그림자 텍스쳐를 업데이트 한다.
void cShadow1::UpdateShadow(cRenderer &renderer, cNode &node)
{
	RET(!m_surface.IsLoaded());

	const Vector3 pos = node.GetTransform().GetPosition();

	// 전역 광원으로 부터 그림자 생성에 필요한 정보를 얻어온다.
	Vector3 lightPos;
	Matrix44 view, proj, tt;
	cLightManager::Get()->GetMainLight().GetShadowMatrix(
		pos, lightPos, view, proj, tt );

	if (node.m_shader)
	{
		node.m_shader->SetMatrix("g_mView", view);
		node.m_shader->SetMatrix("g_mProj", proj);
	}

	Begin(renderer);
	node.RenderShadow(renderer, view*proj, lightPos, Vector3(0,-1,0), Matrix44::Identity);
	End(renderer);
}


void cShadow1::UpdateShadow(cRenderer &renderer, cModel2 &model)
{
	RET(!m_surface.IsLoaded());

	const Vector3 pos = model.m_tm.GetPosition();

	// 전역 광원으로 부터 그림자 생성에 필요한 정보를 얻어온다.
	Vector3 lightPos;
	Matrix44 view, proj, tt;
	cLightManager::Get()->GetMainLight().GetShadowMatrix(
		pos, lightPos, view, proj, tt);

	if (model.m_shader)
	{
		model.m_shader->SetMatrix("g_mView", view);
		model.m_shader->SetMatrix("g_mProj", proj);
	}

	Begin(renderer);
	model.RenderShader(renderer);
	End(renderer);
}


void cShadow1::Bind(cShader &shader, const string &key)
{

}


void cShadow1::Begin(cRenderer &renderer)
{
	m_surface.Begin(renderer);
	renderer.GetDevice()->Clear(0, NULL
		, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
		, 0x00000000, 1.0f, 0L);
}


void cShadow1::End(cRenderer &renderer)
{
	m_surface.End(renderer);
}


// 그림자 맵 출력. (디버깅용)
void cShadow1::RenderShadowMap(cRenderer &renderer)
{
	m_surface.Render(renderer);
}


void cShadow1::LostDevice()
{
	m_surface.LostDevice();
}


void cShadow1::ResetDevice(graphic::cRenderer &renderer)
{
	m_surface.ResetDevice(renderer);
}
