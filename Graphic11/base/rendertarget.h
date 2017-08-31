//
// 2017-08-30, jjuiddong
// DX11 RenderTarget
//	- RenderTargetView + DepthStencilView
//
#pragma once


namespace graphic
{

	class cRenderTarget
	{
	public:
		cRenderTarget();
		virtual ~cRenderTarget();

		bool Create(cRenderer &renderer
			, const cViewport &viewPort
			, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM
			, const DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
		);

		void SetRenderTarget(cRenderer &renderer);
		void RecoveryRenderTarget(cRenderer &renderer);
		void Bind(cRenderer &renderer, const int stage = 0);

		void Clear();


	public:
		cViewport m_viewPort;

		ID3D11ShaderResourceView *m_texture;
		ID3D11Texture2D *m_rawTex;
		ID3D11RenderTargetView *m_renderTargetView;
		ID3D11DepthStencilView *m_depthStencilView;
	};


}