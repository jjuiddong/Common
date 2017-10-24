
#include "stdafx.h"
#include "rendertarget.h"

using namespace graphic;


cRenderTarget::cRenderTarget()
	: m_texRTV(NULL)
	, m_texSRV(NULL)
	, m_depthDSV(NULL)
	, m_depthSRV(NULL)
	, m_texture(NULL)
	, m_resolvedTex(NULL)
	, m_resolvedSRV(NULL)
{
}

cRenderTarget::~cRenderTarget()
{
	Clear();
}


bool cRenderTarget::Create(cRenderer &renderer
	, const cViewport &viewPort
	, const DXGI_FORMAT rtvFormat //render target view = DXGI_FORMAT_R8G8B8A8_UNORM
	, const bool isMultiSampling //= true
	, const bool isDepthStencil //= true
	, const DXGI_FORMAT depthFormat //depth stecil view = DXGI_FORMAT_D24_UNORM_S8_UINT
	, const DXGI_FORMAT depthSRVFormat //= DXGI_FORMAT_D24_UNORM_S8_UINT
	, const DXGI_FORMAT depthDSVFormat //= DXGI_FORMAT_D24_UNORM_S8_UINT
)
{
	Clear();

	m_rtvFormat = rtvFormat;
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
	desc.SampleDesc.Count = isMultiSampling? 4 : 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(renderer.GetDevice()->CreateTexture2D(&desc, NULL, &m_texture)))
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = rtvFormat;
	rtvDesc.ViewDimension = isMultiSampling? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	if (FAILED(renderer.GetDevice()->CreateRenderTargetView(m_texture, &rtvDesc, &m_texRTV)))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC rdesc;
	ZeroMemory(&rdesc, sizeof(rdesc));
	rdesc.Format = rtvFormat;
	rdesc.ViewDimension = isMultiSampling? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	rdesc.Texture2D.MipLevels = 1;

	if (FAILED(renderer.GetDevice()->CreateShaderResourceView(m_texture, &rdesc, &m_texSRV)))
		return false;

	if (isMultiSampling)
	{
		desc.SampleDesc.Count = 1;
		if (FAILED(renderer.GetDevice()->CreateTexture2D(&desc, NULL, &m_resolvedTex)))
			return false;

		rdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		if (FAILED(renderer.GetDevice()->CreateShaderResourceView(m_resolvedTex, &rdesc, &m_resolvedSRV)))
			return false;
	}


	// Create depth stencil texture
	if (isDepthStencil)
	{
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = depthFormat;
		descDepth.SampleDesc.Count = isMultiSampling? 4 : 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		ID3D11Texture2D *depthTexture = NULL;
		HRESULT hr = renderer.GetDevice()->CreateTexture2D(&descDepth, NULL, &depthTexture);
		if (FAILED(hr))
			return false;

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthDSVFormat;
		descDSV.ViewDimension = isMultiSampling ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = renderer.GetDevice()->CreateDepthStencilView(depthTexture, &descDSV, &m_depthDSV);
		if (FAILED(hr))
			return false;

		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
		ZeroMemory(&descSRV, sizeof(descSRV));
		descSRV.Format = depthSRVFormat;
		descSRV.ViewDimension = isMultiSampling ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MipLevels = 1;
		descSRV.Texture2D.MostDetailedMip = 0;

		if (FAILED(renderer.GetDevice()->CreateShaderResourceView(depthTexture, &descSRV, &m_depthSRV)))
			return false;

		SAFE_RELEASE(depthTexture);
	}

	return true;
}


void cRenderTarget::SetRenderTarget(cRenderer &renderer) 
{
	renderer.SetRenderTarget(m_texRTV, m_depthDSV);
	m_viewPort.Bind(renderer);
}


void cRenderTarget::RecoveryRenderTarget(cRenderer &renderer)
{
	renderer.SetRenderTarget(NULL, NULL);
	renderer.m_viewPort.Bind(renderer);
}


bool cRenderTarget::Begin(cRenderer &renderer
	, const Vector4 &color //= Vector4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.0f)
	, const bool isClear //= true
)
{
	renderer.SetRenderTarget(m_texRTV, m_depthDSV);

	m_viewPort.Bind(renderer);
	
	if (isClear)
	{
		if (renderer.ClearScene(false, color))
		{
			renderer.BeginScene();
			return true;
		}
	}
	else
	{
		renderer.BeginScene();
		return true;
	}

	return false;
}


bool cRenderTarget::BeginSimple(cRenderer &renderer
	, const Vector4 &color //= Vector4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.0f)
	, const bool isClear //= true
)
{
	m_viewPort.Bind(renderer);
	renderer.GetDevContext()->OMSetRenderTargets( 1, &m_texRTV, m_depthDSV);
	if (isClear)
	{
		renderer.GetDevContext()->ClearRenderTargetView(m_texRTV, (float*)&color);

		if (m_depthDSV)
			renderer.GetDevContext()->ClearDepthStencilView(m_depthDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );
	}

	return false;
}


void cRenderTarget::End(cRenderer &renderer)
{
	renderer.EndScene();
	renderer.SetRenderTarget(NULL, NULL);
	renderer.m_viewPort.Bind(renderer);

	if (m_resolvedSRV)
	{
		renderer.GetDevContext()->ResolveSubresource(
			m_resolvedTex, 0, m_texture, 0, m_rtvFormat);
	}
}


void cRenderTarget::Bind(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->PSSetShaderResources(stage, 1, &m_texSRV);
}


void cRenderTarget::Clear()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_resolvedTex);
	SAFE_RELEASE(m_texSRV);
	SAFE_RELEASE(m_texRTV);
	SAFE_RELEASE(m_depthDSV);
	SAFE_RELEASE(m_depthSRV);
	SAFE_RELEASE(m_resolvedSRV);
}
