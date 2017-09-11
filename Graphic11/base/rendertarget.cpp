
#include "stdafx.h"
#include "rendertarget.h"

using namespace graphic;


cRenderTarget::cRenderTarget()
	: m_renderTargetView(NULL)
	, m_depthStencilView(NULL)
	, m_texture(NULL)
	, m_rawTex(NULL)
{
}

cRenderTarget::~cRenderTarget()
{
	Clear();
}


bool cRenderTarget::Create(cRenderer &renderer
	, const cViewport &viewPort
	, const DXGI_FORMAT rtvFormat //render target view = DXGI_FORMAT_R8G8B8A8_UNORM
	, const DXGI_FORMAT dsvFormat //depth stecil view = DXGI_FORMAT_D24_UNORM_S8_UINT
)
{
	Clear();

	m_viewPort = viewPort;
	const int width = (int)viewPort.m_vp.Width;
	const int height = (int)viewPort.m_vp.Height;

	// Create Render Target Texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = rtvFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(renderer.GetDevice()->CreateTexture2D(&desc, NULL, &m_rawTex)))
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = rtvFormat;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	if (FAILED(renderer.GetDevice()->CreateRenderTargetView(m_rawTex, &rtvDesc, &m_renderTargetView)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC rdesc;
	ZeroMemory(&rdesc, sizeof(rdesc));
	rdesc.Format = rtvFormat;
	rdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rdesc.Texture2D.MipLevels = 1;

	if (FAILED(renderer.GetDevice()->CreateShaderResourceView(m_rawTex, &rdesc, &m_texture)))
		return false;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = dsvFormat;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D *pDepthStencil = NULL;
	HRESULT hr = renderer.GetDevice()->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = renderer.GetDevice()->CreateDepthStencilView(pDepthStencil, &descDSV, &m_depthStencilView);
	if (FAILED(hr))
		return false;

	pDepthStencil->Release();

	return true;
}


void cRenderTarget::SetRenderTarget(cRenderer &renderer) 
{
	renderer.SetRenderTarget(m_renderTargetView, m_depthStencilView);
	m_viewPort.Bind(renderer);
}


void cRenderTarget::RecoveryRenderTarget(cRenderer &renderer)
{
	renderer.SetRenderTarget(NULL, NULL);
	renderer.m_viewPort.Bind(renderer);
}


void cRenderTarget::Bind(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->PSSetShaderResources(stage, 1, &m_texture);
}


void cRenderTarget::Clear()
{
	SAFE_RELEASE(m_rawTex);
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_depthStencilView);
}
