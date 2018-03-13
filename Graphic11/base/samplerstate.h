//
// 2017-07-31, jjuiddong
// DX11 SamplerState
//
#pragma once


namespace graphic
{
	using namespace common;

	class cSamplerState
	{
	public:
		cSamplerState();
		virtual ~cSamplerState();

		bool Create(cRenderer &renderer
			, const D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR
			, const D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_WRAP
			, const D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_WRAP
			, const D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_WRAP
		);

		void Bind(cRenderer &renderer, const int startSlot = 0);
		void Clear();


	public:
		ID3D11SamplerState *m_sampler;
	};

}
