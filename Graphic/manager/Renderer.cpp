
#include "stdafx.h"
#include "Renderer.h"
#include "../base/DxInit.h"
#include "resourcemanager.h"

using namespace graphic;


void graphic::ReleaseRenderer()
{
	//cRenderer::Release();
	cResourceManager::Release();
	cMainCamera::Release();
	cLightManager::Release();
	cFontManager::Release();
	cPickManager::Release();
	cInputManager::Release();
}



////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer

cRenderer::cRenderer() :
	m_pDevice(NULL)
,	m_elapseTime(0)
,	m_fps(0)
{
	m_postRender.reserve(128);
}

cRenderer::~cRenderer()
{
	SAFE_RELEASE(m_pDevice);
}


// DirectX Device 객체 생성.
bool cRenderer::CreateDirectX(HWND hWnd, const int width, const int height)
{
	if (!InitDirectX(hWnd, width, height, m_params, m_pDevice))
		return false;

	m_width = width;
	m_height = height;

	m_textFps.Create(*this);
	m_textFps.SetPos(0, 0);
	//m_textFps.SetColor(D3DXCOLOR(0,0,0,1));
	m_textFps.SetColor(D3DXCOLOR(255, 255, 255, 1));

	m_hWnd = hWnd;
	return true;
}


// x, y, z 축을 출력한다.
void cRenderer::RenderAxis()
{
	RET(!m_pDevice);

	if (m_axis.empty())
		MakeAxis(500.f,  D3DXCOLOR(1,0,0,0),  D3DXCOLOR(0,1,0,0),  D3DXCOLOR(0,0,1,0), m_axis);


	// 가장 위에 출력되기 위해서 zbuffer 를 끈다.
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, 0);

	DWORD lighting;
	m_pDevice->GetRenderState( D3DRS_LIGHTING, &lighting );
	m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	m_pDevice->SetTexture(0, NULL);
	Matrix44 identity;
	m_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&identity );
	m_pDevice->SetFVF( sVertexDiffuse::FVF );
	m_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, 3, &m_axis[0], sizeof(sVertexDiffuse) );
	m_pDevice->SetRenderState( D3DRS_LIGHTING, lighting );

	//m_pDevice->SetRenderState(D3DRS_ZENABLE, 1);
}


// FPS 출력.
void cRenderer::RenderFPS()
{
	//RET(!m_font);
	m_textFps.Render();
	//RECT rc = {10,10,200,200};
	//m_font->DrawTextA( NULL, m_fpsText.c_str(), -1, &rc,
	//	DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
}


// 그리드 출력.
void cRenderer::RenderGrid()
{
	static int gridSize = 0;
	if (m_grid.empty())
	{
		MakeGrid(10, 64, D3DXCOLOR(0.8f,0.8f,0.8f,1), m_grid);
		gridSize = m_grid.size() / 2;
	}

	if (gridSize > 0)
	{
		// 가장 위에 출력되기 위해서 zbuffer 를 끈다.
		m_pDevice->SetRenderState(D3DRS_ZENABLE, 0);

		DWORD lighting;
		m_pDevice->GetRenderState( D3DRS_LIGHTING, &lighting );
		m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pDevice->SetTexture(0, NULL);
		Matrix44 identity;
		m_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&identity );
		m_pDevice->SetFVF( sVertexDiffuse::FVF );
		m_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, gridSize, &m_grid[0], sizeof(sVertexDiffuse) );
		m_pDevice->SetRenderState( D3DRS_LIGHTING, lighting);
		m_pDevice->SetRenderState(D3DRS_ZENABLE, 1);
	}
}



void cRenderer::Update(const float elapseT)
{
	// fps 계산 ---------------------------------------
	++m_fps;
	m_elapseTime += elapseT;
	if( 1.f <= m_elapseTime )
	{
		m_textFps.SetText(format("fps: %d", m_fps));
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
		out.reserve(count * 4);
		const Vector3 start(count/2 * -width, 0, count/2 * width);

		for (int i=0; i < count+1; ++i)
		{
			sVertexDiffuse vtx;
			vtx.p = start;
			vtx.p.x += (i * width);
			vtx.c = color;
			out.push_back(vtx);

			vtx.p += Vector3(0, 0.001f,-width*count);
			out.push_back(vtx);
		}

		for (int i=0; i < count+1; ++i)
		{
			sVertexDiffuse vtx;
			vtx.p = start;
			vtx.p.z -= (i * width);
			vtx.c = color;
			out.push_back(vtx);

			vtx.p += Vector3(width*count, 0.001f, 0);
			out.push_back(vtx);
		}
	}
}


// x, y, z 축을 만든다.
void cRenderer::MakeAxis( const float length, DWORD xcolor, DWORD ycolor, DWORD zcolor, 
	vector<sVertexDiffuse> &out )
{
	RET(!out.empty());

	out.reserve(6);

	sVertexDiffuse v;

	// x axis
	v.p = Vector3( 0.f, 0.001f, 0.f );
	v.c = xcolor;
	out.push_back(v);

	v.p = Vector3( length, 0.001f, 0.f );
	v.c = xcolor;
	out.push_back(v);

	// y axis
	v.p = Vector3( 0.f, 0.001f, 0.f );
	v.c = ycolor;
	out.push_back(v);

	v.p = Vector3( 0.f, length, 0.f );
	v.c = ycolor;
	out.push_back(v);

	// z axis
	v.p = Vector3( 0.f, 0.001f, 0.f );
	v.c = zcolor;
	out.push_back(v);

	v.p = Vector3( 0.f, 0.001f, length );
	v.c = zcolor;
	out.push_back(v);
}


bool cRenderer::ClearScene()
{
	HRESULT hr = GetDevice()->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
		return false;
	else if (hr == D3DERR_DEVICENOTRESET)// reset상황.				
		return false;

	if (SUCCEEDED(GetDevice()->Clear(
		0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		//D3DCOLOR_XRGB(150, 150, 150), 
		D3DCOLOR_ARGB(0, 66, 75, 121),
		1.0f, 0)))
	{
		return true;
	}

	return false;
}


void cRenderer::BeginScene()
{
	GetDevice()->BeginScene();
}


void cRenderer::Present()
{
	GetDevice()->Present(NULL, NULL, NULL, NULL);
}


void cRenderer::EndScene()
{
	// Post Render Object
	for (auto &p : m_postRender)
		p.obj->Render(*this, p.opt);
	m_postRender.clear();

	GetDevice()->EndScene();
}


bool cRenderer::CheckResetDevice(const int width, const int height)
{
	if ((m_params.BackBufferWidth == width) && (m_params.BackBufferHeight == height))
		return false;
	return true;
}


bool cRenderer::ResetDevice(const int width, const int height)
{
	if (!CheckResetDevice(width, height))
		return false;

	cResourceManager::Get()->LostDevice();
	m_textFps.LostDevice();

	m_width = width;
	m_height = height;
	m_params.BackBufferWidth = width;
	m_params.BackBufferHeight = height;

	HRESULT hr = GetDevice()->Reset(&m_params);
	if (FAILED(hr))
	{
		switch (hr)
		{
		//장치를 손실했지만 리셋하지 못했고 그래서 랜더링 불가
		case D3DERR_DEVICELOST:
			OutputDebugStringA("OnResetDevice 장치를 손실했지만 리셋하지 못했고 그래서 랜더링 불가 \n");
			break;
			//내부 드라이버 에러,어플리케이션은 이 메세지 일때 일반적으로 셧다운된다.
		case D3DERR_DRIVERINTERNALERROR:
			OutputDebugStringA("OnResetDevice 드라이버 내부 에러,어플리케이션은 이 메세지 일때 일반적으로 셧다운된다.");
			break;
			//부적절한 호출이다. 파라메터가 잘못일수도 있고..
		case D3DERR_INVALIDCALL:
			OutputDebugStringA("OnResetDevice 부적절한 호출. 파라메터가 잘못된경우.. ");
			break;
			//디스플레이 메모리가 충분하지 않음
		case D3DERR_OUTOFVIDEOMEMORY:
			OutputDebugStringA("OnResetDevice 디스플레이 메모리가 충분하지 않음 \n");
			break;
			//Direct3D가 이호출에 대한 메모리 확보를 못햇다.
		case E_OUTOFMEMORY:
			OutputDebugStringA("OnResetDevice Direct3D가 이호출에 대한 메모리 확보를 못햇다. \n");
			break;
		}
		Sleep(10);
		return false;
	}

	const Vector3 lookAt = GetMainCamera()->GetLookAt();
	const Vector3 eyePos = GetMainCamera()->GetEyePos();
	GetMainCamera()->SetCamera(eyePos, lookAt, Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(GetMainCamera()->m_fov,
		(float)width / (float)height,
		GetMainCamera()->m_nearPlane, GetMainCamera()->m_farPlane);
	GetMainCamera()->SetViewPort(width, height);

	cResourceManager::Get()->ResetDevice(*this);

	return true;
}


void cRenderer::AddPostRender(iRenderable *obj, const int opt)
// opt=0
{
	m_postRender.push_back({ opt, obj });
}


void cRenderer::SetCullMode(const D3DCULL cull)
{
	GetDevice()->SetRenderState(D3DRS_CULLMODE, cull);
}
void cRenderer::SetFillMode(const D3DFILLMODE mode)
{
	GetDevice()->SetRenderState(D3DRS_FILLMODE, mode);
}
void cRenderer::SetNormalizeNormals(const bool value)
{
	GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, value? TRUE : FALSE);
}
void cRenderer::SetAlphaBlend(const bool value)
{
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, value ? TRUE : FALSE);
}

void cRenderer::SetZEnable(const bool value)
{
	GetDevice()->SetRenderState(D3DRS_ZENABLE, value ? TRUE : FALSE);
}


D3DFILLMODE cRenderer::GetFillMode()
{
	DWORD value;
	GetDevice()->GetRenderState(D3DRS_FILLMODE, &value);
	return (D3DFILLMODE)value;
}
