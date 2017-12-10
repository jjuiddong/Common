
#include "stdafx.h"
#include "Renderer.h"
#include "../base/DxInit.h"

using namespace graphic;


void graphic::ReleaseRenderer()
{
	cResourceManager::Release();
	cMainCamera::Release();
	cLightManager::Release();
}



////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer

cRenderer::cRenderer() 
	: m_elapseTime(0)
	, m_isMainRenderer(true)
	, m_isThreadRender(false)
	, m_d3dDevice(NULL)
	, m_devContext(NULL)
	, m_swapChain(NULL)
	, m_renderTargetView(NULL)
	, m_depthStencilView(NULL)
	, m_refRTV(NULL)
	, m_refDSV(NULL)
	, m_fps(0)
	, m_isDbgRender(false)
	, m_dbgRenderStyle(0)
	, m_textGenerateCount(0)
	, m_textCacheCount(0)
	, m_drawCallCount(0)
	, m_preDrawCallCount(0)
{
	ZeroMemory(m_textureMap, sizeof(m_textureMap));
}

cRenderer::~cRenderer()
{
	m_textMgr.Clear();

	for (auto &p : m_alphaSpace)
		delete p;
	m_alphaSpace.clear();

	for (auto &p : m_alphaSpaceBuffer)
		delete p;
	m_alphaSpaceBuffer.clear();

	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_depthStencilView);
	SAFE_RELEASE(m_swapChain);
	m_refRTV = NULL;
	m_refDSV = NULL;

	if (m_isMainRenderer)
		SAFE_RELEASE(m_d3dDevice);

	if (m_isMainRenderer || m_isThreadRender) // main renderer or deferred context
		SAFE_RELEASE(m_devContext);

	m_d3dDevice = NULL;
}


// DirectX Device 객체 생성.
bool cRenderer::CreateDirectX(const bool isThreadRender, HWND hWnd, const float width, const float height)
{
	if (!InitDirectX11(hWnd, width, height, &m_d3dDevice, &m_devContext
		, &m_swapChain, &m_renderTargetView, &m_depthStencilView))
		return false;

	m_isMainRenderer = true;
	m_isThreadRender = isThreadRender;

	InitRenderer(hWnd, width, height);

	return true;
}


bool cRenderer::CreateDirectXSubRenderer(const bool isThreadRender, HWND hWnd, const float width, const float height
	, ID3D11Device *device, ID3D11DeviceContext *devContext )
{
	const bool isCreateImmediateMode = !isThreadRender;

	if (isCreateImmediateMode)
	{
		if (!InitDirectX11SwapChain(device, hWnd, width, height 
			, &m_swapChain, &m_renderTargetView, &m_depthStencilView))
			return false;
	}
	else
	{
		if (!InitDirectX11Deferred(device, hWnd, width, height, &m_devContext
			, &m_swapChain, &m_renderTargetView, &m_depthStencilView))
			return false;
	}

	m_d3dDevice = device;
	m_devContext = devContext;

	m_isMainRenderer = false;
	m_isThreadRender = isThreadRender;

	InitRenderer(hWnd, width, height);

	return true;
}


// Initialize Renderer
void cRenderer::InitRenderer(HWND hWnd, const float width, const float height)
{
	m_hWnd = hWnd;

	m_viewPort.Create(0, 0, width, height, 0, 1.f);
	m_viewPort.Bind(*this);

	m_cbPerFrame.Create(*this);
	m_cbLight.Create(*this);
	m_cbMaterial.Create(*this);
	m_cbInstancing.Create(*this);
	m_cbClipPlane.Create(*this);
	m_cbSkinning.Create(*this);
	float f2[4] = { 1, 1, 1, 100000 }; // default clipplane always positive return
	memcpy(m_cbClipPlane.m_v->clipPlane, f2, sizeof(f2));

	m_textMgr.Create(256);

	//---------------------------------------------------------
	// Initialize Shader
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos.fxo", eVertexType::POSITION);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-color.fxo", eVertexType::POSITION | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos_rhw-color.fxo", eVertexType::POSITION_RHW | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-tex.fxo", eVertexType::POSITION | eVertexType::TEXTURE);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm.fxo", eVertexType::POSITION | eVertexType::NORMAL);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos_rhw-color-tex.fxo"
		, eVertexType::POSITION_RHW | eVertexType::TEXTURE | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-color.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-tex.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-color-tex.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, "../media/shader11/water.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE, false);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-tex-bump.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE | eVertexType::TANGENT | eVertexType::BINORMAL);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-tex-skin.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE | eVertexType::BLENDINDICES | eVertexType::BLENDWEIGHT);
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-tex-skin-bump.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE 
		| eVertexType::TANGENT | eVertexType::BINORMAL
		| eVertexType::BLENDINDICES | eVertexType::BLENDWEIGHT);

	// POSITION => R32G32B32A32 4개를 사용함.
	D3D11_INPUT_ELEMENT_DESC skyboxcube_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/skyboxcube.fxo", skyboxcube_layout, ARRAYSIZE(skyboxcube_layout), false);


	m_textFps.Create(*this, 20, true, "Arial", cColor::WHITE);
	m_line.Create(*this);
	m_rect3D.Create(*this, 32);
	m_sphere.Create(*this, 1, 10, 10);

	cBoundingBox bbox(Vector3(0, 0, 0), Vector3(1, 1, 1)*0.2f, Quaternion());
	m_dbgBox.Create(*this, bbox, cColor::RED);
	m_dbgArrow.Create(*this, Vector3(0, 0, 0), Vector3(1, 1, 1));
	m_dbgLine.Create(*this, Vector3(0, 0, 0), Vector3(1, 1, 1), 1, cColor::WHITE);
	m_dbgSphere.Create(*this, 1.f, 10, 10, cColor::WHITE);
	m_line2D.Create(*this);

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_dbgAxis.Create(*this);
	m_dbgAxis.SetAxis(bbox2, false);

	m_defaultMtrl.InitWhite();
}


// x, y, z 축을 출력한다.
void cRenderer::RenderAxis()
{
	m_dbgAxis.Render(*this);
}


// FPS 출력.
void cRenderer::RenderFPS()
{
	m_textFps.Render(*this, 70, 17);
}


// 그리드 출력.
void cRenderer::RenderGrid()
{

}


void cRenderer::Update(const float elapseT)
{
	// fps 계산 ---------------------------------------
	++m_fps;
	m_elapseTime += elapseT;
	if( 1.f <= m_elapseTime )
	{
		m_textFps.SetText(formatw("fps: %d", m_fps).c_str());
		m_fps = 0;
		m_elapseTime = 0;
	}
	//--------------------------------------------------

	m_preDrawCallCount = m_drawCallCount;
	m_drawCallCount = 0;

#ifdef _DEBUG
	m_preShadersDrawCall = m_shadersDrawCall;
#endif
	m_shadersDrawCall.clear();
}


bool cRenderer::ClearScene(
	const bool updateRenderTarget //=true
	, const Vector4 &color //= Vector4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.0f)
)
{
	if (updateRenderTarget)
		SetRenderTarget(m_renderTargetView, m_depthStencilView);

	if (m_refRTV)
		m_devContext->ClearRenderTargetView(m_refRTV, (float*)&color);
	if (m_refDSV)
		m_devContext->ClearDepthStencilView(m_refDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return true;
}


void cRenderer::BeginScene()
{
	AddAlphaBlendSpace(cBoundingBox());
}


void cRenderer::Present()
{
	m_swapChain->Present(0, 0);
}


void cRenderer::ExecuteCommandList(ID3D11CommandList *cmdList)
{
	m_devContext->ExecuteCommandList(cmdList, false);
}


void cRenderer::FinishCommandList()
{
	SAFE_RELEASE(m_cmdList);
	m_devContext->FinishCommandList(false, &m_cmdList);
}


void cRenderer::SetRenderTarget(ID3D11RenderTargetView *renderTargetView, ID3D11DepthStencilView *depthStencilView)
{
	ID3D11RenderTargetView *rtv = (renderTargetView) ? renderTargetView : m_renderTargetView;
	ID3D11DepthStencilView *dsv = (depthStencilView) ? depthStencilView : m_depthStencilView;
	m_refRTV = rtv;
	m_refDSV = dsv;
	m_devContext->OMSetRenderTargets(1, &rtv, dsv);
}


void cRenderer::SetRenderTargetDepth(ID3D11DepthStencilView *depthStencilView)
{
	ID3D11DepthStencilView *dsv = (depthStencilView) ? depthStencilView : m_depthStencilView;
	m_refRTV = NULL;
	m_refDSV = dsv;
	m_devContext->OMSetRenderTargets(0, NULL, dsv);
}


void cRenderer::EndScene()
{
	// Text Render
	m_textMgr.Render(*this);

	cCamera &cam = GetMainCamera();
	const Ray ray = cam.GetRay();

	// AlphaBlending Render, Sorting Camera Position
	// Descent Distance from Camera
	for (auto &space : m_alphaSpace)
	{
		std::sort(space->renders.begin(), space->renders.end(),
			[&](const sRenderObj &a, const sRenderObj &b)
			{
				// 최상위 출력(NoDepth)과 비교 할 경우, 최상위 출력이 항상 나중에 출력되게 한다.
				if (a.p->IsRenderFlag(eRenderFlag::NODEPTH)
					&& !b.p->IsRenderFlag(eRenderFlag::NODEPTH))
				{
					return true;
				}

				if (!a.p->IsRenderFlag(eRenderFlag::NODEPTH)
					&& b.p->IsRenderFlag(eRenderFlag::NODEPTH))
				{
					return false;
				}

				const Vector3 c1 = a.p->m_transform.pos * a.tm;
				const Vector3 c2 = b.p->m_transform.pos * b.tm;
				const Plane plane1(a.p->m_alphaNormal, c1);
				const Plane plane2(b.p->m_alphaNormal, c2);
				const Vector3 dir1 = (c1 - ray.orig).Normal();
				const Vector3 dir2 = (c2 - ray.orig).Normal();

				Vector3 p1 = plane1.Pick(ray.orig, dir2);
				if (a.p->m_alphaRadius *2 < (p1 - c1).Length())
					p1 = plane1.Pick(ray.orig, dir1);

				Vector3 p2 = plane2.Pick(ray.orig, dir1);
				if (b.p->m_alphaRadius *2 < (p2 - c2).Length())
					p2 = plane2.Pick(ray.orig, dir2);

				const float l1 = p1.LengthRoughly(ray.orig);
				const float l2 = p2.LengthRoughly(ray.orig);
				return l1 > l2;
			}
		);
	}

	// Sorting AlphaBlending Space
	std::sort(m_alphaSpace.begin(), m_alphaSpace.end(),
		[&](const sAlphaBlendSpace *a, const sAlphaBlendSpace *b)
		{
			const Vector3 c1 = a->bbox.Center();
			const Vector3 c2 = b->bbox.Center();
			const Vector3 dir1 = (c1 - ray.orig).Normal();
			const Vector3 dir2 = (c2 - ray.orig).Normal();

			float l1;
			if (!a->bbox.Pick(ray.orig, dir2, &l1))
				a->bbox.Pick(ray.orig, dir1, &l1);

			float l2;
			if (!b->bbox.Pick(ray.orig, dir1, &l2))
				b->bbox.Pick(ray.orig, dir2, &l2);

			return l1 > l2;
		}
	);

	for (auto &p : m_alphaSpace)
		for (auto &data : p->renders)
			if (!data.p->IsRenderFlag(eRenderFlag::NODEPTH))
				data.p->Render(*this, data.tm.GetMatrixXM(), data.p->m_renderFlags);

	// NoDepth 옵션을 가장 나중에 출력한다.
	for (auto &p : m_alphaSpace)
		for (auto &data : p->renders)
			if (data.p->IsRenderFlag(eRenderFlag::NODEPTH))
				data.p->Render(*this, data.tm.GetMatrixXM(), data.p->m_renderFlags);

	for (auto &p : m_alphaSpace)
	{
		p->renders.clear();
		m_alphaSpaceBuffer.push_back(p);
	}
	m_alphaSpace.clear();

	m_textGenerateCount = m_textMgr.m_generateCount;
	m_textCacheCount = m_textMgr.m_cacheCount;
	m_textMgr.NewFrame();
}


bool cRenderer::CheckResetDevice(
	const float width //=0
	, const float height //=0
)
{
	float w, h;
	if ((width == 0) || (height == 0))
	{
		sRecti cr;
		GetClientRect(m_hWnd, (RECT*)&cr);
		w = (float)cr.Width();
		h = (float)cr.Height();
	}
	else
	{
		w = width;
		h = height;
	}

	if ((m_viewPort.m_vp.Width == w) && (m_viewPort.m_vp.Height == h))
		return false;

	return true;
}


bool cRenderer::ResetDevice(
	const float width //=0
	, const float height //=0
	, const bool forceReset //=false
	, const bool resetResource //= true
)
{
	float w, h;
	if ((width == 0) || (height == 0))
	{
		sRecti cr;
		GetClientRect(m_hWnd, (RECT*)&cr);
		w = (float)cr.Width();
		h = (float)cr.Height();
	}
	else
	{
		w = width;
		h = height;
	}

	if (!forceReset && !CheckResetDevice(w, h))
		return false;

	m_viewPort.m_vp.Width = w;
	m_viewPort.m_vp.Height = h;
	m_viewPort.Bind(*this);

	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_depthStencilView);
	m_refRTV = NULL;
	m_refDSV = NULL;

	const UINT chW = static_cast<UINT>(w);
	const UINT chH = static_cast<UINT>(h);
	HRESULT hr = m_swapChain->ResizeBuffers(
		2, // Double-buffered swap chain.
		chW,
		chH,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0
	);

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return false;

	hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_renderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return false;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = chW;
	descDepth.Height = chH;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	OUT ID3D11Texture2D *pDepthStencil = NULL;
	hr = m_d3dDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_d3dDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &m_depthStencilView);
	if (FAILED(hr))
		return false;
	pDepthStencil->Release();

	SetRenderTarget(m_renderTargetView, m_depthStencilView);

	const Vector3 lookAt = GetMainCamera().GetLookAt();
	const Vector3 eyePos = GetMainCamera().GetEyePos();
	GetMainCamera().SetCamera(eyePos, lookAt, Vector3(0, 1, 0));
	GetMainCamera().SetViewPort(w, h);

	return true;
}


// 알파 블렌딩 노드를, AlphaBlend Space에 추가한다.
void cRenderer::AddRenderAlphaAll(cNode *node
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	RET(!node);
	RET(!node->m_isEnable);
	RET(!node->IsVisible());

	if (node->IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		Matrix44 tm = parentTm;
		AddRenderAlpha(node, tm);
	}

	for (auto &p : node->m_children)
		AddRenderAlphaAll(p);
}


void cRenderer::AddRenderAlpha(cNode *node
	, const Matrix44 &tm // = Matrix44::Identity
	, const int opt // = 1
)
{
	assert(!m_alphaSpace.empty());
	m_alphaSpace.back()->renders.push_back({ opt, tm, node });
}


void cRenderer::AddRenderAlpha(sAlphaBlendSpace *space
	, cNode *node
	, const Matrix44 &tm // = Matrix44::Identity
	, const int opt // = 1
)
{
	space->renders.push_back({ opt, tm, node });
}


sAlphaBlendSpace* cRenderer::AddAlphaBlendSpace(const cBoundingBox &bbox)
{
	if (m_alphaSpaceBuffer.empty())
	{
		sAlphaBlendSpace *pNew = new sAlphaBlendSpace;
		pNew->renders.reserve(256);	
		m_alphaSpaceBuffer.push_back(pNew);
	}

	sAlphaBlendSpace *p = m_alphaSpaceBuffer.back();
	p->renders.clear();
	m_alphaSpaceBuffer.pop_back();

	p->bbox = bbox;
	m_alphaSpace.push_back(p);
	return p;
}


sAlphaBlendSpace* cRenderer::GetCurrentAlphaBlendSpace()
{
	assert(!m_alphaSpace.empty());
	return m_alphaSpace.back();
}


void cRenderer::BindTexture(cTexture *texture, const int stage)
{
	RET(MAX_TEXTURE_STAGE <= (stage - 1));
	m_textureMap[stage - TEXTURE_OFFSET] = (texture) ? texture->m_texSRV : NULL;
}


void cRenderer::BindTexture(cRenderTarget &rt, const int stage)
{
	RET(MAX_TEXTURE_STAGE <= (stage - 1));
	m_textureMap[stage - TEXTURE_OFFSET] = rt.m_texSRV;
}


void cRenderer::BindTexture(cDepthBuffer &db, const int stage)
{
	RET(MAX_TEXTURE_STAGE <= (stage - 1));
	m_textureMap[stage - TEXTURE_OFFSET] = db.m_depthSRV;
}


void cRenderer::UnbindTexture(const int stage)
{
	RET(MAX_TEXTURE_STAGE <= (stage - 1));
	m_textureMap[stage - TEXTURE_OFFSET] = NULL;
}


void cRenderer::UnbindTextureAll()
{
	ZeroMemory(m_textureMap, sizeof(m_textureMap));

	ID3D11ShaderResourceView *ns[MAX_TEXTURE_STAGE] = { NULL, NULL, NULL, NULL, NULL, NULL };
	GetDevContext()->PSSetShaderResources(0, MAX_TEXTURE_STAGE, ns);
}
