
#include "stdafx.h"
#include "water.h"

using namespace graphic;


cWater::cWater()
	: cNode(common::GenerateId(), "Water", eNodeType::MODEL)
	, m_isRenderSurface(false)
{
	Matrix44 mWaterWorld;
	mWaterWorld.SetTranslate(Vector3(0, 0.5f, 0));

	sInitInfo waterInitInfo;
	waterInitInfo.dirLight.Init(cLight::LIGHT_DIRECTIONAL);
	waterInitInfo.mtrl.InitWhite();
	waterInitInfo.vertRows = 64;
	waterInitInfo.vertCols = 64;
	waterInitInfo.cellSize = 64.f;
	waterInitInfo.dx = 1.0f;
	waterInitInfo.dz = 1.0f;
	waterInitInfo.uvFactor = 512;
	waterInitInfo.yOffset = 0.f;
	waterInitInfo.waveMapFilename0 = "wave0.png";
	waterInitInfo.waveMapFilename1 = "wave1.png";
	waterInitInfo.waveMapVelocity0 = Vector2(0.09f, 0.06f);
	waterInitInfo.waveMapVelocity1 = Vector2(-0.05f, 0.08f);
	waterInitInfo.texScale = 10.0f; 
	waterInitInfo.refractBias = 0.1f;
	waterInitInfo.refractPower = 2.0f;
	waterInitInfo.rippleScale  = Vector2(0.06f, 0.03f); 
	//waterInitInfo.rippleScale  = Vector2(0.06f, 0.0f); 
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
	vp.Create(0, 0, 1024, 1024, 0.f, 1.f);
	m_reflectMap.Create(renderer, vp, DXGI_FORMAT_R8G8B8A8_UNORM, false);
	m_refractMap.Create(renderer, vp, DXGI_FORMAT_R8G8B8A8_UNORM, false);

	m_grid.Create(renderer
		, m_initInfo.vertRows, m_initInfo.vertCols, m_initInfo.cellSize, m_initInfo.cellSize
		, (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0)
		, cColor::WHITE
		, g_defaultTexture
		//, "sora.jpg"
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

	renderer.BindTexture(m_waveMap0, 4);
	renderer.BindTexture(m_waveMap1, 5);
	renderer.BindTexture(m_reflectMap, 6);
	renderer.BindTexture(m_refractMap, 7);

	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	Vector3 dirW(0.0f, -1.0f, -3.0f);
	dirW.Normalize();
	renderer.m_cbLight.m_v->direction = XMLoadFloat3((XMFLOAT3*)&dirW);

	const Matrix44 worldInv = m_initInfo.toWorld.Inverse();
	m_cbWater.m_v->gWorldInv = XMMatrixTranspose(worldInv.GetMatrixXM());
	m_cbWater.m_v->gRefractBias = m_initInfo.refractBias;
	m_cbWater.m_v->gRefractPower = m_initInfo.refractPower;
	m_cbWater.m_v->gRippleScale = *(XMFLOAT2*)&m_initInfo.rippleScale;
	m_cbWater.m_v->gWaveMapOffset0 = *(XMFLOAT2*)&m_waveMapOffset0;
	m_cbWater.m_v->gWaveMapOffset1 = *(XMFLOAT2*)&m_waveMapOffset1;

	renderer.m_cbMaterial = m_initInfo.mtrl.GetMaterial();
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_initInfo.toWorld.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer, 0);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.Update(renderer, 2);
	m_cbWater.Update(renderer, 3);

	m_grid.m_vtxBuff.Bind(renderer);
	m_grid.m_idxBuff.Bind(renderer);

	if ((m_grid.m_vtxType & eVertexType::TEXTURE0) && m_grid.m_texture)
		m_grid.m_texture->Bind(renderer, 0);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	//renderer.GetDevContext()->OMSetBlendState(states.AlphaBlend(), 0, 0xffffffff);

	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_grid.m_idxBuff.GetFaceCount() * 3, 0, 0);
	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

	//m_grid.Render(renderer);

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
	renderer.ClearScene(false);
	renderer.BeginScene();

	// Reflection plane in world space.
	Plane waterPlaneW = GetWorldPlane();
	float f[4] = { waterPlaneW.N.x, waterPlaneW.N.y, waterPlaneW.N.z, waterPlaneW.D };

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
	memcpy(renderer.m_cbClipPlane.m_v->clipPlane, f, sizeof(f));
}


void cWater::EndRefractScene(cRenderer &renderer)
{
	renderer.EndScene();
	m_refractMap.RecoveryRenderTarget(renderer);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);

	float f2[4] = { 1,1,1,1000 }; // default clipplane always positive return
	memcpy(renderer.m_cbClipPlane.m_v->clipPlane, f2, sizeof(f2));
}


void cWater::BeginReflectScene(cRenderer &renderer)
{
	m_reflectMap.SetRenderTarget(renderer);
	renderer.ClearScene(false);
	renderer.BeginScene();

	// Reflection plane in world space.
	Plane waterPlaneW = GetWorldPlane();
	float f[4] = { waterPlaneW.N.x, waterPlaneW.N.y, waterPlaneW.N.z, waterPlaneW.D };

	CommonStates states(renderer.GetDevice());
	memcpy(renderer.m_cbClipPlane.m_v->clipPlane, f, sizeof(f));
	renderer.GetDevContext()->RSSetState(states.CullClockwise());
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
}


void cWater::EndReflectScene(cRenderer &renderer)
{
	renderer.EndScene();
	m_reflectMap.RecoveryRenderTarget(renderer);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);

	float f2[4] = { 1,1,1,1000 }; // default clipplane always positive return
	memcpy(renderer.m_cbClipPlane.m_v->clipPlane, f2, sizeof(f2));
}


Plane cWater::GetWorldPlane()
{
	// Reflection plane in local space.
	Plane waterPlaneL(0, -1, 0, 0);

	// Reflection plane in world space.
	Matrix44 waterWorld;
	waterWorld.SetTranslate(Vector3(0, m_initInfo.toWorld.GetPosition().y, 0)); // water height
	Matrix44 WInvTrans;
	WInvTrans = waterWorld.Inverse();
	WInvTrans.Transpose();
	Plane waterPlaneW = waterPlaneL * WInvTrans;
	return waterPlaneW;
}


Matrix44 cWater::GetReflectionMatrix()
{
	return GetWorldPlane().GetReflectMatrix();
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
