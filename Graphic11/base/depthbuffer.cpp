
#include "stdafx.h"
#include "depthbuffer.h"

using namespace graphic;

cDepthBuffer::cDepthBuffer()
	: m_depthDSV(NULL)
	, m_texture(NULL)
	, m_depthSRV(NULL)
{
}

cDepthBuffer::~cDepthBuffer()
{
	Clear();
}


bool cDepthBuffer::Create(cRenderer &renderer
	, const cViewport &viewPort
	, const bool isMultiSampling //= true
	, const DXGI_FORMAT texFormat //depth stecil view = DXGI_FORMAT_R32_TYPELESS
	, const DXGI_FORMAT SRVFormat //depth stecil view = DXGI_FORMAT_R32_FLOAT
	, const DXGI_FORMAT DSVFormat //depth stecil view = DXGI_FORMAT_D32_FLOAT
)
{
	Clear();

	m_viewPort = viewPort;
	const int width = (int)viewPort.m_vp.Width;
	const int height = (int)viewPort.m_vp.Height;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = texFormat;
	descDepth.SampleDesc.Count = isMultiSampling ? 4 : 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	HRESULT hr = renderer.GetDevice()->CreateTexture2D(&descDepth, NULL, &m_texture);
	RETV2(FAILED(hr), false);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DSVFormat;
	descDSV.ViewDimension = isMultiSampling ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = renderer.GetDevice()->CreateDepthStencilView(m_texture, &descDSV, &m_depthDSV);
	RETV2(FAILED(hr), false);

	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	ZeroMemory(&descSRV, sizeof(descSRV));
	descSRV.Format = SRVFormat;
	descSRV.ViewDimension = isMultiSampling ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MipLevels = 1;
	descSRV.Texture2D.MostDetailedMip = 0;

	hr = renderer.GetDevice()->CreateShaderResourceView(m_texture, &descSRV, &m_depthSRV);
	RETV2(FAILED(hr), false);

	return true;
}


void cDepthBuffer::SetRenderTarget(cRenderer &renderer)
{
	renderer.SetRenderTargetDepth(m_depthDSV);
	m_viewPort.Bind(renderer);
}


void cDepthBuffer::RecoveryRenderTarget(cRenderer &renderer)
{
	renderer.SetRenderTarget(NULL, NULL);
	renderer.m_viewPort.Bind(renderer);
}


bool cDepthBuffer::Begin(cRenderer &renderer
	, const bool isClear //= true
	, const Vector4 &clearColor // = Vector4(1,1,1,1)
)
{
	renderer.SetRenderTargetDepth(m_depthDSV);

	m_viewPort.Bind(renderer);

	if (isClear)
	{
		if (renderer.ClearScene(false, clearColor))
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


void cDepthBuffer::End(cRenderer &renderer)
{
	renderer.EndScene();
	renderer.SetRenderTarget(NULL, NULL);
	renderer.m_viewPort.Bind(renderer);
}


void cDepthBuffer::Bind(cRenderer &renderer
	, const int stage //= 0
)
{
	renderer.GetDevContext()->PSSetShaderResources(stage, 1, &m_depthSRV);
}


void cDepthBuffer::Clear()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_depthSRV);
	SAFE_RELEASE(m_depthDSV);
}
