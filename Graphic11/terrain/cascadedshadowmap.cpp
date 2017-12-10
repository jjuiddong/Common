
#include "stdafx.h"
#include "cascadedshadowmap.h"

using namespace graphic;

cCascadedShadowMap::cCascadedShadowMap()
	: m_shadowMapSize(1024,1024)
	, m_lightCams{cCamera3D("shadow camera1"), cCamera3D("shadow camera2"), cCamera3D("shadow camera3") }
{
	m_splitZ[0] = 0.0003f;
	m_splitZ[1] = 0.001f;
	m_splitZ[2] = 0.003f;
}

cCascadedShadowMap::~cCascadedShadowMap()
{
}


bool cCascadedShadowMap::Create(cRenderer &renderer
	, const Vector2 &shadowMapSize //= Vector2(1024, 1024)
	, const float z0 //= 30
	, const float z1 //= 100
	, const float z2 //= 300
)
{
	m_shadowMapSize = shadowMapSize;
	m_splitZ[0] = z0;
	m_splitZ[1] = z1;
	m_splitZ[2] = z2;

	cViewport svp = renderer.m_viewPort;
	svp.m_vp.MinDepth = 0.f;
	svp.m_vp.MaxDepth = 1.f;
	svp.m_vp.Width = shadowMapSize.x;
	svp.m_vp.Height = shadowMapSize.y;

	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		m_shadowMaps[i].Create(renderer, svp, false, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_D32_FLOAT);
		m_frustums[i].Create(renderer, GetMainCamera().GetViewProjectionMatrix());
		m_lightCams[i].SetProjectionOrthogonal(shadowMapSize.x, shadowMapSize.y, 1, 1000000);
		m_dbgLightFrustums[i].Create(renderer, m_lightCams[i].GetViewProjectionMatrix());
		m_shadowMapQuads[i].Create(renderer, 0, 310.f * i, 300.f, 300.f);
	}

	return true;
}


bool cCascadedShadowMap::UpdateParameter(cRenderer &renderer, const cCamera &camera)
{
	// Split MainCamera to Divided Space Camera (Orthogonal Space)
	cFrustum frustums[3];
	cFrustum::Split3_2(camera, m_splitZ[0], m_splitZ[1], m_splitZ[2]
		, frustums[0], frustums[1], frustums[2]);

	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		m_lightCams[i].SetCamera2(GetMainLight().m_pos, GetMainLight().m_direction, Vector3(0, 1, 0));
		m_lightCams[i].FitFrustum(frustums[i].m_viewProj);
		m_frustums[i].SetFrustum(renderer, m_lightCams[i].GetViewProjectionMatrix());
		m_dbgLightFrustums[i].SetFrustum(renderer, m_lightCams[i].GetViewProjectionMatrix());
	}
	
	return true;
}


// Prepare Render ShadowMap
bool cCascadedShadowMap::Begin(cRenderer &renderer, const int shadowMapIndex
	, const bool isClear //= true
)
{
	RETV2(shadowMapIndex >= SHADOWMAP_COUNT, false);

	cMainCamera::Get()->PushCamera(&m_lightCams[shadowMapIndex]);
	m_lightCams[shadowMapIndex].Bind(renderer);
	renderer.UnbindTextureAll();
	m_shadowMaps[shadowMapIndex].Begin(renderer, isClear);

	return true;
}


// End Render ShadowMap
bool cCascadedShadowMap::End(cRenderer &renderer, const int shadowMapIndex)
{
	RETV2(shadowMapIndex >= SHADOWMAP_COUNT, false);

	m_shadowMaps[shadowMapIndex].End(renderer);
	cMainCamera::Get()->PopCamera();
	return true;
}


bool cCascadedShadowMap::Bind(cRenderer &renderer)
{
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		Matrix44 view, proj, tt;
		m_lightCams[i].GetShadowMatrix(view, proj, tt);
		renderer.m_cbPerFrame.m_v->mLightView[i] = XMMatrixTranspose(view.GetMatrixXM());
		renderer.m_cbPerFrame.m_v->mLightProj[i] = XMMatrixTranspose(proj.GetMatrixXM());
		renderer.m_cbPerFrame.m_v->mLightTT = XMMatrixTranspose(tt.GetMatrixXM());
	}

	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
		renderer.BindTexture(m_shadowMaps[i], i + cRenderer::TEXTURE_OFFSET);

	return true;
}


void cCascadedShadowMap::DebugRender(cRenderer &renderer)
{
	for (int i = 0; i < 3; ++i)
	{
		cTexture shadowTex(m_shadowMaps[i].m_depthSRV);
		m_shadowMapQuads[i].m_texture = &shadowTex;
		m_shadowMapQuads[i].Render(renderer);
		//m_frustum[i].RenderShader(renderer);
		m_dbgLightFrustums[i].Render(renderer);
	}
}


void cCascadedShadowMap::BuildShadowMap(cRenderer &renderer, cNode *node
	, const XMMATRIX &parentTm // = XMIdentity
	, const bool isClear //= true;
)
{
	node->SetTechnique("BuildShadowMap");

	UpdateParameter(renderer, GetMainCamera());
	for (int i = 0; i < cCascadedShadowMap::SHADOWMAP_COUNT; ++i)
	{
		node->CullingTest(m_frustums[i], parentTm, true);

		Begin(renderer, i, isClear);
		node->Render(renderer, parentTm, eRenderFlag::SHADOW);
		End(renderer, i);
	}

	cFrustum frustum;
	frustum.SetFrustum(GetMainCamera().GetViewProjectionMatrix());
	node->CullingTest(frustum, parentTm, true); // Recovery Culling
}


void cCascadedShadowMap::RenderShadowMap(cRenderer &renderer, cNode *node
	, const XMMATRIX &parentTm // = XMIdentity
)
{
	node->SetTechnique("ShadowMap");
	Bind(renderer);
	node->Render(renderer, parentTm, eRenderFlag::TERRAIN);
	node->Render(renderer, parentTm, eRenderFlag::MODEL | eRenderFlag::NOALPHABLEND);
}
