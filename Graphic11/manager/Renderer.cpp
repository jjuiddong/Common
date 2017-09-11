
#include "stdafx.h"
#include "Renderer.h"
#include "../base/DxInit.h"
//#include "resourcemanager.h"

using namespace graphic;


void graphic::ReleaseRenderer()
{
	cResourceManager::Release();
	cMainCamera::Release();
	//cLightManager::Release();
	//cFontManager::Release();
	//cPickManager::Release();
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
{
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
bool cRenderer::CreateDirectX(const bool isThreadRender, HWND hWnd, const int width, const int height)
{
	if (!InitDirectX11(hWnd, width, height, &m_d3dDevice, &m_devContext
		, &m_swapChain, &m_renderTargetView, &m_depthStencilView))
		return false;

	m_isMainRenderer = true;
	m_isThreadRender = isThreadRender;

	InitRenderer(hWnd, width, height);

	return true;
}


bool cRenderer::CreateDirectXSubRenderer(const bool isThreadRender, HWND hWnd, const int width, const int height
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
void cRenderer::InitRenderer(HWND hWnd, const int width, const int height)
{
	m_hWnd = hWnd;

	m_viewPort.Create(0, 0, (float)width, (float)height, 0, 1.f);
	m_viewPort.Bind(*this);

	m_cbPerFrame.Create(*this);
	m_cbLight.Create(*this);
	m_cbMaterial.Create(*this);
	m_cbInstancing.Create(*this);
	m_cbInstancingTemp.Create(*this);

	m_textMgr.Create(256);

	//---------------------------------------------------------
	// Initialize Shader
	D3D11_INPUT_ELEMENT_DESC pos_color[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-color.fxo", pos_color, ARRAYSIZE(pos_color));

	D3D11_INPUT_ELEMENT_DESC posrhw_color[] =
	{
		{ "POSITION_RHW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos_rhw-color.fxo", posrhw_color, ARRAYSIZE(posrhw_color));

	D3D11_INPUT_ELEMENT_DESC pos_tex[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-tex.fxo", pos_tex, ARRAYSIZE(pos_tex));

	D3D11_INPUT_ELEMENT_DESC posrhw_color_tex[] =
	{
		{ "POSITION_RHW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos_rhw-color-tex.fxo", posrhw_color_tex, ARRAYSIZE(posrhw_color_tex));

	D3D11_INPUT_ELEMENT_DESC pos_norm_color[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-color.fxo", pos_norm_color, ARRAYSIZE(pos_norm_color));

	D3D11_INPUT_ELEMENT_DESC pos_norm_tex[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-tex.fxo", pos_norm_tex, ARRAYSIZE(pos_norm_tex));

	D3D11_INPUT_ELEMENT_DESC pos_norm_color_tex[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shaderMgr.LoadShader(*this, "../media/shader11/pos-norm-color-tex.fxo", pos_norm_color_tex, ARRAYSIZE(pos_norm_color_tex));

	m_shaderMgr.LoadShader(*this, "../media/shader11/water.fxo", pos_norm_tex, ARRAYSIZE(pos_norm_tex), false);


	m_textFps.Create(*this, 20, true, "Arial", cColor::BLUE);
	//m_textFps.SetPos(0, 0);
	//m_textFps.SetColor(D3DXCOLOR(255, 255, 255, 1));

	//m_dbgBox.SetBox(*this, Vector3(1, 1, 1)*-0.2f, Vector3(1, 1, 1)*0.2f);
	//m_dbgBox.SetColor(D3DCOLOR_XRGB(255, 0, 0));
	//m_dbgArrow.Create(*this, Vector3(0, 0, 0), Vector3(1, 1, 1));
	//m_dbgSphere.Create(*this, 1, 10, 10);
	//m_dbgAxis.Create(*this);
}


// x, y, z 축을 출력한다.
void cRenderer::RenderAxis()
{
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
}


bool cRenderer::ClearScene(
	const bool updateRenderTarget //=true
	, const Vector4 &color //= Vector4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.0f)
)
{
	if (updateRenderTarget)
		SetRenderTarget(m_renderTargetView, m_depthStencilView);

	float ClearColor[4] = { 50.f/255.f, 50.f / 255.f, 50.f / 255.f, 1.0f }; // red,green,blue,alpha
	//m_devContext->ClearRenderTargetView(m_refRTV, ClearColor);
	m_devContext->ClearRenderTargetView(m_refRTV, (float*)&color);
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


void cRenderer::EndScene()
{
	// Text Render
	m_textMgr.Render(*this);

	cCamera &cam = GetMainCamera();
	Vector3 camOrig, camDir;
	cam.GetRay(camOrig, camDir);

	// AlphaBlending Render, Sorting Camera Position
	// Descent Distance from Camera
	for (auto &space : m_alphaSpace)
	{
		std::sort(space->renders.begin(), space->renders.end(),
			[&](const sRenderObj &a, const sRenderObj &b)
			{
				const Vector3 c1 = a.p->m_boundingBox.Center() * a.tm;
				const Vector3 c2 = b.p->m_boundingBox.Center() * b.tm;
				const Plane plane1(a.normal, c1);
				const Plane plane2(b.normal, c2);
				const Vector3 dir1 = (c1 - camOrig).Normal();
				const Vector3 dir2 = (c2 - camOrig).Normal();

				Vector3 p1 = plane1.Pick(camOrig, dir2);
				if (a.p->m_boundingSphere.m_bsphere.Radius*2 < (p1 - c1).Length())
					p1 = plane1.Pick(camOrig, dir1);

				Vector3 p2 = plane2.Pick(camOrig, dir1);
				if (b.p->m_boundingSphere.m_bsphere.Radius*2 < (p2 - c2).Length())
					p2 = plane2.Pick(camOrig, dir2);

				const float l1 = p1.LengthRoughly(camOrig);
				const float l2 = p2.LengthRoughly(camOrig);
				return l1 > l2;
			}
		);
	}

	for (auto &p : m_alphaSpace)
		for (auto &data : p->renders)
			data.p->Render(*this, data.tm.GetMatrixXM(), -1);

	for (auto &p : m_alphaSpace)
	{
		p->renders.clear();
		m_alphaSpaceBuffer.push_back(p);
	}
	m_alphaSpace.clear();
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
		w = cr.Width();
		h = cr.Height();
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

	//if (resetResource)
	//	cResourceManager::Get()->LostDevice();

	//m_textFps.LostDevice();

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

	//if (resetResource)
	//	cResourceManager::Get()->ResetDevice(*this);

	return true;
}


void cRenderer::AddRenderAlpha(cNode2 *node
	, const Vector3 &normal //= Vector3(0, 0, 1),
	, const Matrix44 &tm // = Matrix44::Identity
	, const int opt // = 1
)
{
	assert(!m_alphaSpace.empty());
	m_alphaSpace.back()->renders.push_back({ opt, normal ,tm, node });
}


void cRenderer::AddRenderAlpha(sAlphaBlendSpace *space
	, cNode2 *node
	, const Vector3 &normal //= Vector3(0, 0, 1),
	, const Matrix44 &tm // = Matrix44::Identity
	, const int opt // = 1
)
{
	space->renders.push_back({ opt, normal ,tm, node });
}


void cRenderer::AddAlphaBlendSpace(const cBoundingBox &bbox)
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
}


sAlphaBlendSpace* cRenderer::GetCurrentAlphaBlendSpace()
{
	assert(!m_alphaSpace.empty());
	return m_alphaSpace.back();
}
