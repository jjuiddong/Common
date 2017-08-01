
#include "stdafx.h"
#include "samplerstate.h"


using namespace graphic;


cSamplerState::cSamplerState()
	: m_sampler(NULL)
{
}

cSamplerState::~cSamplerState()
{
	Clear();
}


bool cSamplerState::Create(cRenderer &renderer
	, const D3D11_FILTER filter//= D3D11_FILTER_MIN_MAG_MIP_LINEAR
	, const D3D11_TEXTURE_ADDRESS_MODE addressU //= D3D11_TEXTURE_ADDRESS_WRAP
	, const D3D11_TEXTURE_ADDRESS_MODE addressV //= D3D11_TEXTURE_ADDRESS_WRAP
	, const D3D11_TEXTURE_ADDRESS_MODE addressW //= D3D11_TEXTURE_ADDRESS_WRAP
)
{
	Clear();

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = filter;
	sampDesc.AddressU = addressU;
	sampDesc.AddressV = addressV;
	sampDesc.AddressW = addressW;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(renderer.GetDevice()->CreateSamplerState(&sampDesc, &m_sampler)))
		return false;

	return true;
}


void cSamplerState::Bind(cRenderer &renderer
	, const int startSlot //= 0
)
{
	renderer.GetDevContext()->PSSetSamplers(startSlot, 1, &m_sampler);
}


void cSamplerState::Clear()
{
	SAFE_RELEASE(m_sampler);
}
