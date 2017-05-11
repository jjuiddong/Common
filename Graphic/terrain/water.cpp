
#include "stdafx.h"
#include "water.h"

using namespace graphic;


cWater::cWater()
	: m_isRenderSurface(false)
	, m_isFirstUpdateShader(true)
{
	Matrix44 mWaterWorld;
	//mWaterWorld.SetTranslate(Vector3(0,10,0));
	mWaterWorld.SetTranslate(Vector3(0, 0, 0));

	sInitInfo waterInitInfo;
	waterInitInfo.dirLight.Init(cLight::LIGHT_DIRECTIONAL);
	waterInitInfo.mtrl.InitWhite();
	waterInitInfo.vertRows = 64;
	waterInitInfo.vertCols = 64;
	waterInitInfo.cellSize = 64.f;
	waterInitInfo.dx = 1.0f;
	waterInitInfo.dz = 1.0f;
	waterInitInfo.uvFactor = 8.f;
	waterInitInfo.yOffset = 0.f;
	waterInitInfo.waveMapFilename0 = "wave0.dds";
	waterInitInfo.waveMapFilename1 = "wave1.dds";
	waterInitInfo.waveMapVelocity0 = Vector2(0.09f, 0.06f);
	waterInitInfo.waveMapVelocity1 = Vector2(-0.05f, 0.08f);
	waterInitInfo.texScale = 10.0f; 
	waterInitInfo.refractBias = 0.1f;
	waterInitInfo.refractPower = 2.0f;
	//waterInitInfo.rippleScale  = Vector2(0.06f, 0.03f); 
	waterInitInfo.rippleScale  = Vector2(0.06f, 0.0f); 
	waterInitInfo.toWorld = mWaterWorld;

	m_initInfo = waterInitInfo;

	m_waveMapOffset0 = Vector2(0.0f, 0.0f);
	m_waveMapOffset1 = Vector2(0.0f, 0.0f);

}

cWater::~cWater()
{
}


// 물 클래스 생성.
bool cWater::Create(cRenderer &renderer)
{
	RETV(m_grid.GetVertexBuffer().GetVertexCount() > 0, true); // 이미 생성되었다면 리턴

	const int width = 512;
	const int  height = 512;
	m_reflectMap.Create(renderer, width, height, 0, D3DFMT_X8R8G8B8, true, D3DFMT_D24X8, true, NULL, 0.f, 1.f);
	m_refractMap.Create(renderer, width, height, 0, D3DFMT_X8R8G8B8, true, D3DFMT_D24X8, true, NULL, 0.f, 1.f);

	m_shader = cResourceManager::Get()->LoadShader(renderer, "water.fx");
	if (!m_shader)
		return false;

	m_shader->SetTechnique("WaterTech");

	m_grid.Create(renderer, m_initInfo.vertRows, m_initInfo.vertCols, m_initInfo.cellSize
		, m_initInfo.uvFactor, m_initInfo.yOffset);
	m_grid.GetTexture().Create(renderer, cResourceManager::Get()->FindFile(g_defaultTexture));

	m_waveMap0 = cResourceManager::Get()->LoadTextureParallel(renderer, m_initInfo.waveMapFilename0);
	m_waveMap1 = cResourceManager::Get()->LoadTextureParallel(renderer, m_initInfo.waveMapFilename1);
	cResourceManager::Get()->AddParallelLoader(
		new cParallelLoader(cParallelLoader::eType::TEXTURE, m_initInfo.waveMapFilename0
			, (void**)&m_waveMap0));
	cResourceManager::Get()->AddParallelLoader(
		new cParallelLoader(cParallelLoader::eType::TEXTURE, m_initInfo.waveMapFilename1
			, (void**)&m_waveMap1));

	//m_waveMap0 = cResourceManager::Get()->LoadTexture(renderer, m_initInfo.waveMapFilename0);
	//m_waveMap1 = cResourceManager::Get()->LoadTexture(renderer, m_initInfo.waveMapFilename1);
	//if (!m_waveMap0 || !m_waveMap1)
	//	return false;

	//m_hWVP = m_shader->GetValueHandle( "gWVP");
	//m_hEyePosW = m_shader->GetValueHandle( "gEyePosW");
	//m_hLight = m_shader->GetValueHandle( "gLight");
	//m_hMtrl = m_shader->GetValueHandle( "gMtrl");
	//m_hWaveMapOffset0 = m_shader->GetValueHandle( "gWaveMapOffset0");
	//m_hWaveMapOffset1 = m_shader->GetValueHandle( "gWaveMapOffset1");
	//m_hReflectMap = m_shader->GetValueHandle( "gReflectMap");
	//m_hRefractMap = m_shader->GetValueHandle( "gRefractMap");

	m_initInfo.dirLight.m_light.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1);

	m_initInfo.mtrl.m_mtrl.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1);
	m_initInfo.mtrl.m_mtrl.Specular = D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.6f);
	m_initInfo.mtrl.m_mtrl.Power = 200;

	UpdateShader();

	return true;
}


void cWater::Render(cRenderer &renderer)
{
	cLightManager::Get()->GetMainLight().Bind(*m_shader);

	m_shader->SetMatrix("g_mVP", GetMainCamera()->GetViewProjectionMatrix());
	m_shader->SetMatrix("gWVP", m_initInfo.toWorld*GetMainCamera()->GetViewProjectionMatrix());
	m_shader->SetVector("gEyePosW", GetMainCamera()->GetEyePos());
	m_shader->SetVector("gWaveMapOffset0", m_waveMapOffset0);
	m_shader->SetVector("gWaveMapOffset1", m_waveMapOffset1);

	m_grid.RenderShader(renderer, *m_shader);

	// 디버깅용.
	if (m_isRenderSurface)
	{
		m_refractMap.Render(renderer, 1);
		m_reflectMap.Render(renderer, 2);
	}
}


void cWater::Update(const float elapseTime)
{
	if (m_isFirstUpdateShader && m_waveMap0 && m_waveMap1)
	{
		UpdateShader();
		m_isFirstUpdateShader = false;
	}

	// Update texture coordinate offsets.  These offsets are added to the
	// texture coordinates in the vertex shader to animate them.
	m_waveMapOffset0 += m_initInfo.waveMapVelocity0 * elapseTime;
	m_waveMapOffset1 += m_initInfo.waveMapVelocity1 * elapseTime;

	// Textures repeat every 1.0 unit, so reset back down to zero
	// so the coordinates do not grow too large.
	if(m_waveMapOffset0.x >= 1.0f || m_waveMapOffset0.x <= -1.0f)
		m_waveMapOffset0.x = 0.0f;
	if(m_waveMapOffset1.x >= 1.0f || m_waveMapOffset1.x <= -1.0f)
		m_waveMapOffset1.x = 0.0f;
	if(m_waveMapOffset0.y >= 1.0f || m_waveMapOffset0.y <= -1.0f)
		m_waveMapOffset0.y = 0.0f;
	if(m_waveMapOffset1.y >= 1.0f || m_waveMapOffset1.y <= -1.0f)
		m_waveMapOffset1.y = 0.0f;
}

void cWater::BeginRefractScene()
{
	m_refractMap.Begin();
}


void cWater::EndRefractScene()
{
	m_refractMap.End();
}


void cWater::BeginReflectScene()
{
	m_reflectMap.Begin();
}


void cWater::EndReflectScene()
{
	m_reflectMap.End();
}


void cWater::LostDevice()
{
	m_reflectMap.LostDevice();
	m_refractMap.LostDevice();
}


void cWater::ResetDevice(cRenderer &renderer)
{
	m_reflectMap.ResetDevice(renderer);
	m_refractMap.ResetDevice(renderer);

	UpdateShader();
}


void cWater::UpdateShader() 
{
	// We don't need to set these every frame since they do not change
	m_shader->SetMatrix("gWorld", m_initInfo.toWorld);
	Matrix44 worldInv = m_initInfo.toWorld.Inverse();
	m_shader->SetMatrix("gWorldInv", worldInv);

	if (m_waveMap0)
		m_waveMap0->Bind(*m_shader, "gWaveMap0");
	if (m_waveMap1)
		m_waveMap1->Bind(*m_shader, "gWaveMap1");
	m_reflectMap.Bind(*m_shader, "gReflectMap");
	m_refractMap.Bind(*m_shader, "gRefractMap");

	Vector3 dirW(0.0f, -1.0f, -3.0f);
	dirW.Normalize();
	m_initInfo.dirLight.Bind(*m_shader);
	m_shader->SetVector("g_light.dirW", dirW);

	m_initInfo.mtrl.Bind(*m_shader);

	m_shader->SetFloat("gRefractBias", m_initInfo.refractBias);
	m_shader->SetFloat("gRefractPower", m_initInfo.refractPower);
	m_shader->SetVector("gRippleScale", m_initInfo.rippleScale);
}
