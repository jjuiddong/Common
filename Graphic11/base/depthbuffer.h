//
// 2017-10-20, jjuiddong
// DepthStencil Buffer for ShadowMap
//
#pragma once


namespace graphic
{

	class cDepthBuffer
	{
	public:
		cDepthBuffer();
		virtual ~cDepthBuffer();

		bool Create(cRenderer &renderer
			, const cViewport &viewPort
			, const DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT
			, const bool isMultiSampling = true
		);

		void SetRenderTarget(cRenderer &renderer);
		void RecoveryRenderTarget(cRenderer &renderer);
		bool Begin(cRenderer &renderer, const bool isClear = true);
		void End(cRenderer &renderer);
		void Bind(cRenderer &renderer, const int stage = 0);
		void Clear();


	public:
		cViewport m_viewPort;
		ID3D11Texture2D *m_texture;
		ID3D11ShaderResourceView *m_depthSRV;
		ID3D11DepthStencilView *m_depthDSV;
	};

}
