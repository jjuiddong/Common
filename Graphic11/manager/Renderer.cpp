
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
	, m_backBuffer(NULL)
	, m_refRTV(NULL)
	, m_refDSV(NULL)
	, m_fps(0)
	, m_isDbgRender(false)
	, m_dbgRenderStyle(1)
	, m_textGenerateCount(0)
	, m_textCacheCount(0)
	, m_drawCallCount(0)
	, m_preDrawCallCount(0)
	, m_renderList(NULL)
	, m_alphaSpaceBufferCount(0)
	, m_renderState(nullptr)
{
	ZeroMemory(m_textureMap, sizeof(m_textureMap));
}

cRenderer::~cRenderer()
{
	m_textMgr.Clear();

	SAFE_DELETE(m_renderList);

	//for (auto &p : m_alphaSpace)
	//	delete p;
	//m_alphaSpace.clear();

	for (auto &p : m_alphaSpaceBuffer)
		delete p;
	m_alphaSpaceBuffer.clear();

	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_depthStencilView);
	SAFE_RELEASE(m_backBuffer);
	SAFE_RELEASE(m_swapChain);
	m_refRTV = NULL;
	m_refDSV = NULL;

	if (m_isMainRenderer)
		SAFE_RELEASE(m_d3dDevice);

	if (m_isMainRenderer || m_isThreadRender) // main renderer or deferred context
		SAFE_RELEASE(m_devContext);

	m_d3dDevice = nullptr;
}


// DirectX Device ��ü ����.
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

	m_renderState = CommonStates(m_d3dDevice);

	m_cbPerFrame.Create(*this);
	m_cbLight.Create(*this);
	m_cbMaterial.Create(*this);
	m_cbInstancing.Create(*this);
	m_cbClipPlane.Create(*this);
	m_cbSkinning.Create(*this);
	m_cbTessellation.Create(*this);

	m_cbPerFrame.m_v->outlineColor = Vector4(0.8f, 0, 0, 1).GetVectorXM();
	float f2[4] = { 1, 1, 1, 100000 }; // default clipplane always positive return
	memcpy(m_cbClipPlane.m_v->clipPlane, f2, sizeof(f2));
	m_cbClipPlane.m_v->reflectAlpha[0] = 0.1f;

	//m_textMgr.Create(256);
	m_textMgr.Create(2000, 512);

	m_renderList = new cRenderList();

	//---------------------------------------------------------
	// Initialize Shader
	const StrPath mediaDir = cResourceManager::Get()->GetMediaDirectory();
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos.fxo", eVertexType::POSITION);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-color.fxo", eVertexType::POSITION | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos_rhw.fxo", eVertexType::POSITION_RHW);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos_rhw-color.fxo", eVertexType::POSITION_RHW | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-tex.fxo", eVertexType::POSITION | eVertexType::TEXTURE0);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-norm.fxo", eVertexType::POSITION | eVertexType::NORMAL);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos_rhw-color-tex.fxo"
		, eVertexType::POSITION_RHW | eVertexType::TEXTURE0 | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-norm-color.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-norm-tex.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-norm-color-tex.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0 | eVertexType::COLOR);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/water.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0, false);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-norm-tex-bump.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0 | eVertexType::TANGENT | eVertexType::BINORMAL);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-norm-tex-skin.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0 | eVertexType::BLENDINDICES | eVertexType::BLENDWEIGHT);
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/pos-norm-tex-skin-bump.fxo"
		, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0
		| eVertexType::TANGENT | eVertexType::BINORMAL
		| eVertexType::BLENDINDICES | eVertexType::BLENDWEIGHT);

	// POSITION => R32G32B32A32 4���� �����.
	D3D11_INPUT_ELEMENT_DESC skyboxcube_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_shaderMgr.LoadShader(*this, mediaDir + "shader11/skyboxcube.fxo", skyboxcube_layout, ARRAYSIZE(skyboxcube_layout), false);


	m_textFps.Create(*this, 20, true, "Arial", cColor::WHITE);
	m_line.Create(*this);
	m_rect3D.Create(*this, 32);
	m_sphere.Create(*this, 1, 10, 10);
	m_cylinder.Create(*this, 1, 1, 10);
	m_capsule.Create(*this, 1, 1, 10, 10);
	m_cone.Create(*this, 1, 1, 20, eVertexType::POSITION);
	m_quad.Create(*this, 1, 1, Vector3(0, 0, 0), eVertexType::POSITION | eVertexType::COLOR
		, "", false, cQuadShape::ePlaneType::XZ);
	m_quad.SetRenderFlag(eRenderFlag::ALPHABLEND, true);

	cBoundingBox bbox(Vector3(0, 0, 0), Vector3(1, 1, 1)*0.2f, Quaternion());
	m_cube.Create(*this, bbox);
	m_cube2.Create(*this, bbox, (eVertexType::POSITION | eVertexType::COLOR));
	m_dbgBox.Create(*this, bbox, cColor::RED);
	m_dbgBoxLine.Create(*this, bbox, cColor::WHITE);
	m_dbgArrow.Create(*this, Vector3(0, 0, 0), Vector3(1, 1, 1));
	m_dbgLine.Create(*this, Vector3(0, 0, 0), Vector3(1, 1, 1), 1, cColor::WHITE);
	m_dbgSphere.Create(*this, 1.f, 10, 10, cColor::WHITE);
	m_line2D.Create(*this);

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_dbgAxis.Create(*this);
	m_dbgAxis.SetAxis(bbox2, false);

	m_dbgAxis2.Create(*this);
	cBoundingBox bbox3(Vector3(0, 0, 0), Vector3(10, 0, 10), Quaternion());
	m_dbgAxis2.SetAxis(bbox3, false);

	m_defaultMtrl.InitWhite();
	m_cbMaterial = m_defaultMtrl.GetMaterial();
	m_cbMaterial.Update(*this, 2);
}


// x, y, z ���� ����Ѵ�.
void cRenderer::RenderAxis()
{
	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_dbgAxis.SetAxis(bbox2, false);
	m_dbgAxis.Render(*this);
}


// x, y, z ���� ����Ѵ�.
void cRenderer::RenderAxis2()
{
	m_dbgAxis2.Render(*this);
}


// FPS ���.
void cRenderer::RenderFPS()
{
	m_textFps.Render(*this, 70, 17, true);
}


// �׸��� ���.
void cRenderer::RenderGrid()
{

}


void cRenderer::Update(const float elapseT)
{
	// fps ��� ---------------------------------------
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
	PushAlphaBlendSpace(cBoundingBox());
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


// read config file to debugging renderer
void cRenderer::ReadConfig(const StrPath &fileName)
{
	cConfig config(fileName.c_str());
	m_isDbgRender = config.GetBool("dbgrender", false);
	m_dbgRenderStyle = config.GetInt("dbgrender-style", 0);
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
	if (m_renderList)
		m_renderList->Render(*this);

	// Text Render
	m_textMgr.Render(*this);

	cCamera &cam = GetMainCamera();
	const Ray ray = cam.GetRay();

	// AlphaBlending Render, Sorting Camera Position
	// Descent Distance from Camera
	for (auto &space : m_renderAlphaSpace)
	{
		std::sort(space->renders.begin(), space->renders.end(),
			[&](const sRenderObj &a, const sRenderObj &b)
			{
				// �ֻ��� ���(NoDepth)�� �� �� ���, �ֻ��� ����� �׻� ���߿� ��µǰ� �Ѵ�.
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

				const Matrix44 parentTm = space->parentTm;
				//const Vector3 c1 = a.p->m_transform.pos * a.tm;
				//const Vector3 c2 = b.p->m_transform.pos * b.tm;
				const Vector3 c1 = a.p->m_transform.pos * parentTm;
				const Vector3 c2 = b.p->m_transform.pos * parentTm;
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
	std::sort(m_renderAlphaSpace.begin(), m_renderAlphaSpace.end(),
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

	for (auto &p : m_renderAlphaSpace)
	{
		const XMMATRIX parentTm = p->parentTm.GetMatrixXM();
		for (auto &data : p->renders)
			if (!data.p->IsRenderFlag(eRenderFlag::NODEPTH))
				data.p->Render(*this, parentTm, data.p->m_renderFlags);
	}

	// NoDepth �ɼ��� ���� ���߿� ����Ѵ�.
	for (auto &p : m_renderAlphaSpace)
	{
		const XMMATRIX parentTm = p->parentTm.GetMatrixXM();
		for (auto &data : p->renders)
			if (data.p->IsRenderFlag(eRenderFlag::NODEPTH))
				data.p->Render(*this, parentTm, data.p->m_renderFlags);
	}

	m_alphaSpaceBufferCount = 0;
	m_renderAlphaSpace.clear();
	m_alphaSpaceStack.clear();

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
	SAFE_RELEASE(m_backBuffer);
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
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBuffer);
	if (FAILED(hr))
		return false;

	hr = m_d3dDevice->CreateRenderTargetView(m_backBuffer, NULL, &m_renderTargetView);
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


// ���� ���� ��带, AlphaBlend Space�� �߰��Ѵ�.
void cRenderer::AddRenderAlphaAll(cNode *node
	, const XMMATRIX &parentTm //= XMIdentity
)
{
	RET(!node);
	RET(!node->m_isEnable);
	RET(!node->IsVisible());

	if (node->IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		if (m_alphaSpaceStack.empty())
			return;

		m_alphaSpaceStack.back()->parentTm = parentTm;
		AddRenderAlpha(node);
	}

	for (auto &p : node->m_children)
		AddRenderAlphaAll(p);
}


void cRenderer::AddRenderAlpha(cNode *node
	, const int opt // = 1
)
{
	assert(!m_alphaSpaceStack.empty());
	m_alphaSpaceStack.back()->renders.push_back({ opt, node });
}


void cRenderer::AddRenderAlpha(sAlphaBlendSpace *space
	, cNode *node
	, const int opt // = 1
)
{
	space->renders.push_back({ opt, node });
}


sAlphaBlendSpace* cRenderer::PushAlphaBlendSpace(const cBoundingBox &bbox)
{
	if (m_alphaSpaceBufferCount >= m_alphaSpaceBuffer.size())
	{
		sAlphaBlendSpace *pNew = new sAlphaBlendSpace;
		pNew->renders.reserve(256);	
		m_alphaSpaceBuffer.push_back(pNew);
	}

	sAlphaBlendSpace *p = m_alphaSpaceBuffer[m_alphaSpaceBufferCount++];
	p->renders.clear();
	p->bbox = bbox;

	m_alphaSpaceStack.push_back(p);
	
	// check same alpha space
	if (m_renderAlphaSpace.end() == std::find(
		m_renderAlphaSpace.begin(), m_renderAlphaSpace.end(), p))
	{
		m_renderAlphaSpace.push_back(p);
	}

	return p;
}


void cRenderer::PopAlphaBlendSpace()
{
	assert(!m_alphaSpaceStack.empty());
	return m_alphaSpaceStack.pop_back();
}


sAlphaBlendSpace* cRenderer::GetCurrentAlphaBlendSpace()
{
	assert(!m_alphaSpaceStack.empty());
	return m_alphaSpaceStack.back();
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

	ID3D11ShaderResourceView *ns[MAX_TEXTURE_STAGE] = { NULL, NULL, NULL, NULL, NULL
		, NULL, NULL, NULL, NULL, NULL };
	GetDevContext()->PSSetShaderResources(0, MAX_TEXTURE_STAGE, ns);
}


void cRenderer::UnbindShaderAll()
{
	GetDevContext()->VSSetShader(NULL, NULL, 0);
	GetDevContext()->GSSetShader(NULL, NULL, 0);
	GetDevContext()->CSSetShader(NULL, NULL, 0);
	GetDevContext()->DSSetShader(NULL, NULL, 0);
	GetDevContext()->HSSetShader(NULL, NULL, 0);
	GetDevContext()->PSSetShader(NULL, NULL, 0);
}
