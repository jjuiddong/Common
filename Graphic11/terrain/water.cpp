
#include "stdafx.h"
#include "water.h"

using namespace graphic;


cWater::cWater()
	: cNode2(common::GenerateId(), "Water", eNodeType::MODEL)
	, m_isRenderSurface(false)
{
	Matrix44 mWaterWorld;
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
	waterInitInfo.waveMapFilename0 = "wave0.png";
	waterInitInfo.waveMapFilename1 = "wave1.png";
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


//
bool cWater::Create(cRenderer &renderer)
{
	cViewport vp;
	vp.Create(0, 0, 512, 512, 0.f, 1.f);
	m_reflectMap.Create(renderer, vp);
	m_refractMap.Create(renderer, vp);

	m_grid.Create(renderer
		, m_initInfo.vertRows, m_initInfo.vertCols, m_initInfo.cellSize
		, (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE)
		, cColor::WHITE
		//, g_defaultTexture
		, "sora.jpg"
		, Vector2(0, 0)
		, Vector2(1, 1)
		, m_initInfo.uvFactor);

	m_grid.m_shader = renderer.m_shaderMgr.FindShader("water.fxo");
	if (!m_grid.m_shader)
		return false;

	m_waveMap0 = cResourceManager::Get()->LoadTextureParallel(renderer, m_initInfo.waveMapFilename0);
	m_waveMap1 = cResourceManager::Get()->LoadTextureParallel(renderer, m_initInfo.waveMapFilename1);
	cResourceManager::Get()->AddParallelLoader(
		new cParallelLoader(cParallelLoader::eType::TEXTURE, m_initInfo.waveMapFilename0
			, (void**)&m_waveMap0));
	cResourceManager::Get()->AddParallelLoader(
		new cParallelLoader(cParallelLoader::eType::TEXTURE, m_initInfo.waveMapFilename1
			, (void**)&m_waveMap1));

	m_initInfo.dirLight.m_ambient = Vector4(0.8f, 0.8f, 0.8f, 1);
	m_initInfo.mtrl.m_ambient = Vector4(0.4f, 0.4f, 0.4f, 1);
	m_initInfo.mtrl.m_specular = Vector4(0.6f, 0.6f, 0.6f, 0.6f);
	m_initInfo.mtrl.m_power = 200;

	m_cbWater.Create(renderer);

	return true;
}


bool cWater::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = m_grid.m_shader;
	assert(shader);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_initInfo.toWorld.GetMatrixXM());
	//const Matrix44 worldInv = m_initInfo.toWorld.Inverse();
	//m_cbWater.m_v->gWorldInv = XMMatrixTranspose(worldInv.GetMatrixXM());

	shader->SetBindTexture(m_waveMap0, 1);
	shader->SetBindTexture(m_waveMap1, 2);

	shader->SetBindTexture(m_reflectMap, 3);
	shader->SetBindTexture(m_refractMap, 4);

	Vector3 dirW(0.0f, -1.0f, -3.0f);
	dirW.Normalize();
	renderer.m_cbLight.m_v->direction = XMLoadFloat3((XMFLOAT3*)&dirW);

	m_cbWater.m_v->gRefractPower = m_initInfo.refractBias;
	m_cbWater.m_v->gRefractPower = m_initInfo.refractPower;
	m_cbWater.m_v->gRippleScale = *(XMFLOAT2*)&m_initInfo.rippleScale;
	m_cbWater.m_v->gWaveMapOffset0 = *(XMFLOAT2*)&m_waveMapOffset0;
	m_cbWater.m_v->gWaveMapOffset1 = *(XMFLOAT2*)&m_waveMapOffset1;

	renderer.m_cbPerFrame.Update(renderer, 0);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.Update(renderer, 2);
	m_cbWater.Update(renderer, 3);

	m_grid.Render(renderer);

	//if (m_isRenderSurface)
	//{
	//	m_refractMap.Render(renderer, 1);
	//	m_reflectMap.Render(renderer, 2);
	//}

	return true;
}


bool cWater::Update(cRenderer &renderer, const float deltaSeconds)
{
	// Update texture coordinate offsets.  These offsets are added to the
	// texture coordinates in the vertex shader to animate them.
	m_waveMapOffset0 += m_initInfo.waveMapVelocity0 * deltaSeconds;
	m_waveMapOffset1 += m_initInfo.waveMapVelocity1 * deltaSeconds;

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

	return true;
}


void cWater::BeginRefractScene(cRenderer &renderer)
{
	m_refractMap.SetRenderTarget(renderer);
}


void cWater::EndRefractScene(cRenderer &renderer)
{
	m_refractMap.RecoveryRenderTarget(renderer);
}


void cWater::BeginReflectScene(cRenderer &renderer)
{
	m_reflectMap.SetRenderTarget(renderer);
}


void cWater::EndReflectScene(cRenderer &renderer)
{
	m_reflectMap.RecoveryRenderTarget(renderer);
}


void cWater::LostDevice()
{
	//m_reflectMap.LostDevice();
	//m_refractMap.LostDevice();
}


void cWater::ResetDevice(cRenderer &renderer)
{
	//m_reflectMap.ResetDevice(renderer);
	//m_refractMap.ResetDevice(renderer);
}
