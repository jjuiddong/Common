
#include "stdafx.h"
#include "surface.h"

using namespace graphic;


cSurface::cSurface() : 
	m_width(0)
,	m_height(0)
,	m_texture(NULL)
,	m_surface(NULL)
,	m_zbuffer(NULL)
{

}

cSurface::~cSurface()
{
	Clear();
}


// 렌더타겟 서피스를 생성한다.
// 텍스쳐, 타겟 서피스, Z버퍼 생성.
bool cSurface::CreateRenderTarget(cRenderer &renderer, const int width, const int height)
{
	m_width = width;
	m_height = height;

	if (FAILED(renderer.GetDevice()->CreateTexture(width, height, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT, &m_texture, NULL)))
		return false;

	if (FAILED(m_texture->GetSurfaceLevel(0, &m_surface)))
		return false;

	if (FAILED(renderer.GetDevice()->CreateDepthStencilSurface(
		width, height, D3DFMT_D24S8, 
		D3DMULTISAMPLE_NONE, 0, TRUE,
		&m_zbuffer, NULL)))
		return false;

	return true;
}


// 파일로 저장.
bool cSurface::WritePNGFile(const string &fileName)
{
	if (FAILED(D3DXSaveTextureToFileA(fileName.c_str(), D3DXIFF_PNG, m_texture, NULL)))
	{
		return false;
	}

	return true;
}


// 렌더 타겟을 교체한다.
void cSurface::Begin(cRenderer &renderer)
{
	renderer.GetDevice()->GetRenderTarget(0, &m_oldBackBuffer);
	renderer.GetDevice()->GetDepthStencilSurface(&m_oldZBuffer);
	renderer.GetDevice()->GetViewport(&m_oldViewport);

	renderer.GetDevice()->SetRenderTarget(0, m_surface);
	renderer.GetDevice()->SetDepthStencilSurface(m_zbuffer);
	
	// 뷰포트변경  x y  width    height   minz maxz
	D3DVIEWPORT9 viewport = {0, 0, (DWORD)m_width, (DWORD)m_height, 0.0f, 1.0f};
	renderer.GetDevice()->SetViewport(&viewport);
}


// 렌더 타겟을 원본으로 되돌린다.
void cSurface::End(cRenderer &renderer)
{
	renderer.GetDevice()->SetRenderTarget(0, m_oldBackBuffer);
	renderer.GetDevice()->SetDepthStencilSurface(m_oldZBuffer);
	renderer.GetDevice()->SetViewport(&m_oldViewport);

	SAFE_RELEASE(m_oldBackBuffer);
	SAFE_RELEASE(m_oldZBuffer);
}


// 서피스 출력. 디버깅 용 이다.
void cSurface::Render(cRenderer &renderer)
{
	renderer.GetDevice()->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	renderer.GetDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	float scale = 128.0f;
	typedef struct {FLOAT p[4]; FLOAT tu, tv;} TVERTEX;

	TVERTEX Vertex[4] = {
		// x  y  z rhw tu tv
		{0, scale, 0, 1, 0, 0,},
		{scale, scale,0, 1, 1, 0,},
		{scale, scale+scale,0, 1, 1, 1,},
		{0, scale+scale,0, 1, 0, 1,},
	};
	renderer.GetDevice()->SetTexture(0, m_texture);
	renderer.GetDevice()->SetVertexShader(NULL);
	renderer.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	renderer.GetDevice()->SetPixelShader(0);
	renderer.GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(TVERTEX));
}


// 클래스 초기화.
void cSurface::Clear()
{
	SAFE_RELEASE(m_surface);
	SAFE_RELEASE(m_zbuffer);
	SAFE_RELEASE(m_texture);
}
