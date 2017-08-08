
#include "stdafx.h"
#include "Renderer.h"
#include "../base/DxInit.h"
//#include "resourcemanager.h"

using namespace graphic;


void graphic::ReleaseRenderer()
{
	//cResourceManager::Release();
	cMainCamera::Release();
	//cLightManager::Release();
	//cFontManager::Release();
	//cPickManager::Release();
}



////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer

cRenderer::cRenderer() 
	: m_elapseTime(0)
	, m_d3dDevice(NULL)
	, m_immediateContext(NULL)
	, m_swapChain(NULL)
	, m_renderTargetView(NULL)
	, m_depthStencil(NULL)
	, m_depthStencilView(NULL)
	, m_fps(0)
	, m_isDbgRender(false)
	, m_dbgRenderStyle(0)
{
}

cRenderer::~cRenderer()
{
	//m_textMgr.Clear();

	//for (auto &p : m_alphaSpace)
	//	delete p;
	//m_alphaSpace.clear();

	//for (auto &p : m_alphaSpaceBuffer)
	//	delete p;
	//m_alphaSpaceBuffer.clear();

	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_depthStencil);
	SAFE_RELEASE(m_depthStencilView);
	SAFE_RELEASE(m_swapChain);
	SAFE_RELEASE(m_immediateContext);
	SAFE_RELEASE(m_d3dDevice);

	// Shutdown GDI+
	Gdiplus::GdiplusShutdown(gdiplusToken);
}


// DirectX Device 객체 생성.
bool cRenderer::CreateDirectX(HWND hWnd, const int width, const int height 
	//, const UINT adapter // = D3DADAPTER_DEFAULT
	)
{
	if (!InitDirectX11(hWnd, width, height, 0, &m_d3dDevice, &m_immediateContext
		, &m_swapChain, &m_renderTargetView, &m_depthStencil, &m_depthStencilView))
		return false;

	using namespace Gdiplus;
	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	m_viewPort.Create(0, 0, (float)width, (float)height, 0, 1.f);
	m_viewPort.Bind(*this);

	m_cbPerFrame.Create(*this);
	m_cbLight.Create(*this);
	m_cbMaterial.Create(*this);

	//m_textMgr.Create(256);

	//D3D11_INPUT_ELEMENT_DESC layout1[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};
	//m_vtxLayoutPosDiffuse.Create(*this, layout1, ARRAYSIZE(layout1));


	//m_textFps.Create(*this);
	//m_textFps.SetPos(0, 0);
	//m_textFps.SetColor(D3DXCOLOR(255, 255, 255, 1));

	//m_dbgBox.SetBox(*this, Vector3(1, 1, 1)*-0.2f, Vector3(1, 1, 1)*0.2f);
	//m_dbgBox.SetColor(D3DCOLOR_XRGB(255, 0, 0));
	//m_dbgArrow.Create(*this, Vector3(0, 0, 0), Vector3(1, 1, 1));
	//m_dbgSphere.Create(*this, 1, 10, 10);
	//m_dbgAxis.Create(*this);

	m_hWnd = hWnd;
	return true;
}


// x, y, z 축을 출력한다.
void cRenderer::RenderAxis()
{
	//RET(!m_pDevice);

	//if (m_axis.empty())
	//	MakeAxis(500.f,  D3DXCOLOR(1,0,0,0),  D3DXCOLOR(0,1,0,0),  D3DXCOLOR(0,0,1,0), m_axis);

	// 가장 위에 출력되기 위해서 zbuffer 를 끈다.
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, 0);

	//DWORD lighting;
	//m_pDevice->GetRenderState( D3DRS_LIGHTING, &lighting );
	//m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	//m_pDevice->SetTexture(0, NULL);
	//Matrix44 identity;
	//m_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&identity );
	//m_pDevice->SetFVF( sVertexDiffuse::FVF );
	//m_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, 3, &m_axis[0], sizeof(sVertexDiffuse) );
	//m_pDevice->SetRenderState( D3DRS_LIGHTING, lighting );

	//m_pDevice->SetRenderState(D3DRS_ZENABLE, 1);
}


// FPS 출력.
void cRenderer::RenderFPS()
{
	//m_textFps.Render();
}


// 그리드 출력.
void cRenderer::RenderGrid()
{
	static int gridSize = 0;
	if (m_grid.empty())
	{
		//MakeGrid(10, 64, D3DXCOLOR(0.8f,0.8f,0.8f,1), m_grid);
		//gridSize = m_grid.size() / 2;
	}

	if (gridSize > 0)
	{
		// 가장 위에 출력되기 위해서 zbuffer 를 끈다.
		//m_pDevice->SetRenderState(D3DRS_ZENABLE, 0);

		//DWORD lighting;
		//m_pDevice->GetRenderState( D3DRS_LIGHTING, &lighting );
		//m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		//m_pDevice->SetTexture(0, NULL);
		//Matrix44 identity;
		//m_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&identity );
		//m_pDevice->SetFVF( sVertexDiffuse::FVF );
		//m_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, gridSize, &m_grid[0], sizeof(sVertexDiffuse) );
		//m_pDevice->SetRenderState( D3DRS_LIGHTING, lighting);
		//m_pDevice->SetRenderState(D3DRS_ZENABLE, 1);
	}
}



void cRenderer::Update(const float elapseT)
{
	// fps 계산 ---------------------------------------
	++m_fps;
	m_elapseTime += elapseT;
	if( 1.f <= m_elapseTime )
	{
		//m_textFps.SetText(format("fps: %d", m_fps));
		m_fps = 0;
		m_elapseTime = 0;
	}
	//--------------------------------------------------
}


// 격자무늬 버텍스를 만든다. 
// width = 격자 하나의 폭 크기 (정 사각형이므로 인자값은 하나만 받는다)
// count = 격자 가로 세로 개수
void cRenderer::MakeGrid( const float width, const int count, DWORD color, vector<sVertexDiffuse> &out )
{
	if (out.empty())
	{
		//out.reserve(count * 4);
		//const Vector3 start(count/2 * -width, 0, count/2 * width);

		//for (int i=0; i < count+1; ++i)
		//{
		//	sVertexDiffuse vtx;
		//	vtx.p = start;
		//	vtx.p.x += (i * width);
		//	vtx.c = color;
		//	out.push_back(vtx);

		//	vtx.p += Vector3(0, 0.001f,-width*count);
		//	out.push_back(vtx);
		//}

		//for (int i=0; i < count+1; ++i)
		//{
		//	sVertexDiffuse vtx;
		//	vtx.p = start;
		//	vtx.p.z -= (i * width);
		//	vtx.c = color;
		//	out.push_back(vtx);

		//	vtx.p += Vector3(width*count, 0.001f, 0);
		//	out.push_back(vtx);
		//}
	}
}


// x, y, z 축을 만든다.
void cRenderer::MakeAxis( const float length, DWORD xcolor, DWORD ycolor, DWORD zcolor, 
	vector<sVertexDiffuse> &out )
{
	RET(!out.empty());

	out.reserve(6);

	sVertexDiffuse v;

	//// x axis
	//v.p = Vector3( 0.f, 0.001f, 0.f );
	//v.c = xcolor;
	//out.push_back(v);

	//v.p = Vector3( length, 0.001f, 0.f );
	//v.c = xcolor;
	//out.push_back(v);

	//// y axis
	//v.p = Vector3( 0.f, 0.001f, 0.f );
	//v.c = ycolor;
	//out.push_back(v);

	//v.p = Vector3( 0.f, length, 0.f );
	//v.c = ycolor;
	//out.push_back(v);

	//// z axis
	//v.p = Vector3( 0.f, 0.001f, 0.f );
	//v.c = zcolor;
	//out.push_back(v);

	//v.p = Vector3( 0.f, 0.001f, length );
	//v.c = zcolor;
	//out.push_back(v);
}


bool cRenderer::ClearScene()
{
	//HRESULT hr = GetDevice()->TestCooperativeLevel();
	//if (hr == D3DERR_DEVICELOST)
	//	return false;
	//else if (hr == D3DERR_DEVICENOTRESET)// reset상황.				
	//	return false;

	//if (SUCCEEDED(GetDevice()->Clear(
	//	0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
	//	D3DCOLOR_ARGB(255, 50, 50, 50), // UI Gray
	//	1.0f, 0)))
	//{
	//	return true;
	//}

	float ClearColor[4] = { 50.f/255.f, 50.f / 255.f, 50.f / 255.f, 1.0f }; // red,green,blue,alpha
	m_immediateContext->ClearRenderTargetView(m_renderTargetView, ClearColor);

	m_immediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return true;
}


void cRenderer::BeginScene()
{
	//GetDevice()->BeginScene();

	AddAlphaBlendSpace(cBoundingBox());
}


void cRenderer::Present()
{
	//GetDevice()->Present(NULL, NULL, NULL, NULL);
	m_swapChain->Present(0, 0);
}


void cRenderer::EndScene()
{
	// Text Render
	//m_textMgr.Render(*this);

	cCamera *cam = GetMainCamera();
	Vector3 camOrig, camDir;
	cam->GetRay(camOrig, camDir);

	// AlphaBlending Render, Sorting Camera Position
	// Descent Distance from Camera
	//for (auto &space : m_alphaSpace)
	//{
	//	std::sort(space->renders.begin(), space->renders.end(),
	//		[&](const sRenderObj &a, const sRenderObj &b)
	//		{
	//			const Vector3 c1 = a.p->m_boundingBox.Center() * a.tm;
	//			const Vector3 c2 = b.p->m_boundingBox.Center() * b.tm;
	//			const Plane plane1(a.normal, c1);
	//			const Plane plane2(b.normal, c2);
	//			const Vector3 dir1 = (c1 - camOrig).Normal();
	//			const Vector3 dir2 = (c2 - camOrig).Normal();

	//			Vector3 p1 = plane1.Pick(camOrig, dir2);
	//			if (a.p->m_boundingSphere.m_radius*2 < (p1 - c1).Length())
	//				p1 = plane1.Pick(camOrig, dir1);

	//			Vector3 p2 = plane2.Pick(camOrig, dir1);
	//			if (b.p->m_boundingSphere.m_radius*2 < (p2 - c2).Length())
	//				p2 = plane2.Pick(camOrig, dir2);

	//			const float l1 = p1.LengthRoughly(camOrig);
	//			const float l2 = p2.LengthRoughly(camOrig);
	//			return l1 > l2;
	//		}
	//	);
	//}

	//for (auto &p : m_alphaSpace)
	//	for (auto &data : p->renders)
	//		data.p->Render(*this, data.tm, -1);

	//for (auto &p : m_alphaSpace)
	//{
	//	p->renders.clear();
	//	m_alphaSpaceBuffer.push_back(p);
	//}
	//m_alphaSpace.clear();

	//GetDevice()->EndScene();
}


bool cRenderer::CheckResetDevice(const int width, const int height)
// width=0
// height=0
{
	int w, h;
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

	//if ((m_params.BackBufferWidth == w) && (m_params.BackBufferHeight == h))
	//	return false;
	return true;
}


bool cRenderer::ResetDevice(
	const int width //=0
	, const int height //=0
	, const bool forceReset //=false
	, const bool resetResource //= true
)
{
	int w, h;
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

	if (!forceReset && !CheckResetDevice(w, h))
		return false;

	//if (resetResource)
	//	cResourceManager::Get()->LostDevice();

	//m_textFps.LostDevice();

	m_viewPort.m_vp.Width = (float)w;
	m_viewPort.m_vp.Height = (float)h;
	//m_params.BackBufferWidth = w;
	//m_params.BackBufferHeight = h;

	//HRESULT hr = GetDevice()->Reset(&m_params);
	//if (FAILED(hr))
	//{
	//	switch (hr)
	//	{
	//	//장치를 손실했지만 리셋하지 못했고 그래서 랜더링 불가
	//	case D3DERR_DEVICELOST:
	//		OutputDebugStringA("OnResetDevice 장치를 손실했지만 리셋하지 못했고 그래서 랜더링 불가 \n");
	//		break;
	//		//내부 드라이버 에러,어플리케이션은 이 메세지 일때 일반적으로 셧다운된다.
	//	case D3DERR_DRIVERINTERNALERROR:
	//		OutputDebugStringA("OnResetDevice 드라이버 내부 에러,어플리케이션은 이 메세지 일때 일반적으로 셧다운된다.");
	//		break;
	//		//부적절한 호출이다. 파라메터가 잘못일수도 있고..
	//	case D3DERR_INVALIDCALL:
	//		OutputDebugStringA("OnResetDevice 부적절한 호출. 파라메터가 잘못된경우.. ");
	//		break;
	//		//디스플레이 메모리가 충분하지 않음
	//	case D3DERR_OUTOFVIDEOMEMORY:
	//		OutputDebugStringA("OnResetDevice 디스플레이 메모리가 충분하지 않음 \n");
	//		break;
	//		//Direct3D가 이호출에 대한 메모리 확보를 못햇다.
	//	case E_OUTOFMEMORY:
	//		OutputDebugStringA("OnResetDevice Direct3D가 이호출에 대한 메모리 확보를 못햇다. \n");
	//		break;
	//	}
	//	Sleep(10);
	//	return false;
	//}

	const Vector3 lookAt = GetMainCamera()->GetLookAt();
	const Vector3 eyePos = GetMainCamera()->GetEyePos();
	GetMainCamera()->SetCamera(eyePos, lookAt, Vector3(0, 1, 0));
	GetMainCamera()->SetViewPort(w, h);

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
	//assert(!m_alphaSpace.empty());
	//m_alphaSpace.back()->renders.push_back({ opt, normal ,tm, node });
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
	//if (m_alphaSpaceBuffer.empty())
	//{
	//	sAlphaBlendSpace *pNew = new sAlphaBlendSpace;
	//	pNew->renders.reserve(256);	
	//	m_alphaSpaceBuffer.push_back(pNew);
	//}

	//sAlphaBlendSpace *p = m_alphaSpaceBuffer.back();
	//p->renders.clear();
	//m_alphaSpaceBuffer.pop_back();

	//p->bbox = bbox;
	//m_alphaSpace.push_back(p);
}


sAlphaBlendSpace* cRenderer::GetCurrentAlphaBlendSpace()
{
	//assert(!m_alphaSpace.empty());
	//return m_alphaSpace.back();
	return NULL;
}


//void cRenderer::SetLightEnable(const int light, const bool enable)
//{
//	//GetDevice()->LightEnable(light, enable);
//}
//void cRenderer::SetCullMode(const D3DCULL cull)
//{
//	//GetDevice()->SetRenderState(D3DRS_CULLMODE, cull);
//}
//void cRenderer::SetFillMode(const D3DFILLMODE mode)
//{
//	//GetDevice()->SetRenderState(D3DRS_FILLMODE, mode);
//}
//void cRenderer::SetNormalizeNormals(const bool value)
//{
//	//GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, value? TRUE : FALSE);
//}
//void cRenderer::SetAlphaBlend(const bool value)
//{
//	//GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, value ? TRUE : FALSE);
//}
//
//void cRenderer::SetZEnable(const bool value)
//{
//	//GetDevice()->SetRenderState(D3DRS_ZENABLE, value ? TRUE : FALSE);
//}
//void cRenderer::SetZFunc(const D3DCMPFUNC value)
//{
//	//GetDevice()->SetRenderState(D3DRS_ZFUNC, value);
//}
//
//D3DFILLMODE cRenderer::GetFillMode()
//{
//	DWORD value= 0;
//	//GetDevice()->GetRenderState(D3DRS_FILLMODE, &value);
//	return (D3DFILLMODE)value;
//}
//
//D3DCULL cRenderer::GetCullMode()
//{
//	DWORD value= 0;
//	//GetDevice()->GetRenderState(D3DRS_CULLMODE, &value);
//	return (D3DCULL)value;
//}
