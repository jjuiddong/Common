
#include "stdafx.h"
#include "skybox2.h"

using namespace graphic;


cSkyBox2::cSkyBox2() 
:	m_sphere(NULL)
,	m_envMap(NULL)
{
}

cSkyBox2::~cSkyBox2()
{
	SAFE_RELEASE(m_sphere);
}


bool cSkyBox2::Create(cRenderer &renderer, const StrPath &skyboxFileName, const float radius)
{
	RETV(m_sphere, true);// 이미 생성되었다면 리턴.

	if (FAILED(D3DXCreateSphere(renderer.GetDevice(), radius, 30, 30, &m_sphere, 0)))
	{
		return false;
	}

	m_envMap = cResourceManager::Get()->LoadCubeTexture(renderer, skyboxFileName);
	if (!m_envMap)
		return false;

	m_shader = cResourceManager::Get()->LoadShader(renderer, "sky.fx");
	if (!m_shader)
		return false;
	m_shader->SetTechnique("SkyTech");

	m_radius = radius;
	return true;
}


void cSkyBox2::Render(cRenderer &renderer
	, const Matrix44 &tm//= Matrix44::Identity
)
{
	RET(!m_sphere);
	RET(!m_shader);

	Matrix44 world;
	world.SetTranslate(GetMainCamera()->GetEyePos() );
	world = world * tm * GetMainCamera()->GetViewProjectionMatrix();

	m_shader->SetMatrix("gWVP", world);
	m_envMap->Bind(*m_shader, "gEnvMap");

	m_shader->Begin();
	m_shader->BeginPass();
	m_sphere->DrawSubset(0);
	m_shader->EndPass();
	m_shader->End();
}

