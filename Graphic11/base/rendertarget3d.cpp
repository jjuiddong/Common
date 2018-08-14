
#include "stdafx.h"
#include "rendertarget3d.h"

using namespace graphic;


cRenderTarget3d::cRenderTarget3d()
	: m_rtv(NULL)
	, m_srv(NULL)
	, m_texture(NULL)
{
}

cRenderTarget3d::~cRenderTarget3d()
{
	Clear();
}


bool cRenderTarget3d::Create(cRenderer &renderer
	, const int width, const int height, const int depth
	, const DXGI_FORMAT rtvFormat //render target view = DXGI_FORMAT_R8G8B8A8_UNORM
)
{
	Unbind(renderer);
	Clear();

	m_rtvFormat = rtvFormat;

	// Create Render Target Texture
	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.MipLevels = 1;
	desc.Format = rtvFormat;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(renderer.GetDevice()->CreateTexture3D(&desc, NULL, &m_texture)))
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = rtvFormat;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
	rtvDesc.Texture3D.WSize = -1;
	if (FAILED(renderer.GetDevice()->CreateRenderTargetView(m_texture, &rtvDesc, &m_rtv)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC rdesc;
	ZeroMemory(&rdesc, sizeof(rdesc));
	rdesc.Format = rtvFormat;
	rdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	rdesc.Texture3D.MipLevels = 1;

	if (FAILED(renderer.GetDevice()->CreateShaderResourceView(m_texture, &rdesc, &m_srv)))
		return false;

	return true;
}


void cRenderTarget3d::Bind(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->PSSetShaderResources(stage, 1, &m_srv);
}


void cRenderTarget3d::Unbind(cRenderer &renderer)
{
	ID3D11RenderTargetView *rtv[] = { NULL };
	renderer.GetDevContext()->OMSetRenderTargets(1, rtv, NULL);
}


void cRenderTarget3d::Clear()
{
	SAFE_RELEASE(m_srv);
	SAFE_RELEASE(m_rtv);
	SAFE_RELEASE(m_texture);
}
