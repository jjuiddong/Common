
#include "stdafx.h"
#include "surface2.h"

using namespace graphic;


cSurface2::cSurface2()
	: m_texture(NULL)
	, m_rts(NULL)
	, m_surface(NULL)
	, m_mipLevels(0)
	, m_autoGenMips(0)
{
	ZeroMemory(&m_vp, sizeof(m_vp));
}

cSurface2::~cSurface2()
{
	Clear();
}


bool cSurface2::Create(cRenderer &renderer, 
	const int width, const int height, int mipLevels, 
	D3DFORMAT texFormat,  // D3DFMT_X8R8G8B8
	bool useDepthBuffer, // true
	D3DFORMAT depthFormat, //D3DFMT_D24S8
	bool autoGenMips, // true
	HANDLE *handle, // NULL
	const float minZ, // 1.f, 
	const float maxZ // 10000.f
	) 
{
	m_mipLevels = mipLevels;
	m_texFormat = texFormat;
	m_useDepthBuffer = useDepthBuffer;
	m_depthFormat = depthFormat;
	m_vp.X = 0;
	m_vp.Y = 0;
	m_vp.Width = width;
	m_vp.Height = height;
	m_vp.MinZ = minZ;
	m_vp.MaxZ = maxZ;

	m_autoGenMips = autoGenMips;

	UINT usage = 0;// D3DUSAGE_RENDERTARGET;
	if(m_autoGenMips)
		usage |= D3DUSAGE_AUTOGENMIPMAP;

	if (FAILED(renderer.GetDevice()->CreateTexture(width, height, mipLevels,
		usage, texFormat,
		//(handle)? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT,
		D3DPOOL_MANAGED,
		&m_texture,
		handle)))
	{
		return false;
	}

	if (FAILED(D3DXCreateRenderToSurface(renderer.GetDevice(), 
		width, height, texFormat, useDepthBuffer, depthFormat, &m_rts)))
	{
		return false;
	}

	if (FAILED(m_texture->GetSurfaceLevel(0, &m_surface)))
	{
		return false;
	}
	
	return true;
}


void cSurface2::Begin(cRenderer &renderer)
{
	RET(!m_rts);
	m_rts->BeginScene(m_surface, &m_vp);
}


void cSurface2::End(cRenderer &renderer)
{
	RET(!m_rts);
	m_rts->EndScene(D3DX_FILTER_NONE);
}


void cSurface2::Render(cRenderer &renderer, const int index) // index=1
{
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, ToDxM(Matrix44::Identity));
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	renderer.GetDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	//const float scale = 128.0f;
	const float scale = 256.f;
	typedef struct {FLOAT p[4]; FLOAT tu, tv;} TVERTEX;

	TVERTEX Vertex[4] = {
		// x  y  z rhw tu tv
		{0, scale*index, 0, 1, 0, 0,},
		{scale, scale*index,0, 1, 1, 0,},
		{scale, scale*(index+1),0, 1, 1, 1,},
		{0, scale*(index+1),0, 1, 0, 1,},
	};
	renderer.GetDevice()->SetTexture(0, m_texture);
	renderer.GetDevice()->SetVertexShader(NULL);
	renderer.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	renderer.GetDevice()->SetPixelShader(NULL);
	renderer.GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(TVERTEX));
}


void cSurface2::RenderFull(cRenderer &renderer)
{
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, ToDxM(Matrix44::Identity));
	renderer.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	renderer.GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	renderer.GetDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	renderer.GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	typedef struct { FLOAT p[4]; FLOAT tu, tv; } TVERTEX;
	TVERTEX Vertex[4] = {
		// x  y  z rhw tu tv
		{ 0, 0, 0, 1, 0, 0, },
		{ (float)m_vp.Width, 0,0, 1, 1, 0, },
		{ (float)m_vp.Width, (float)m_vp.Height, 1, 1, 1, 1},
		{ 0, (float)m_vp.Height,0, 1, 0, 1, },
	};
	renderer.GetDevice()->SetTexture(0, m_texture);
	renderer.GetDevice()->SetVertexShader(NULL);
	renderer.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	renderer.GetDevice()->SetPixelShader(NULL);
	renderer.GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(TVERTEX));
}


void cSurface2::Bind(cShader &shader, const string &key)
{
	shader.SetTexture(key, m_texture);
}


void cSurface2::Clear()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_rts);
	SAFE_RELEASE(m_surface);
}


void cSurface2::LostDevice()
{
	Clear();
}


void cSurface2::ResetDevice(cRenderer &renderer)
{
	if (m_vp.Width > 0)
	{
		Create(renderer, m_vp.Width, m_vp.Height, m_mipLevels,
			m_texFormat, m_useDepthBuffer, m_depthFormat, m_autoGenMips, 
			NULL, m_vp.MinZ, m_vp.MaxZ);
	}
}
