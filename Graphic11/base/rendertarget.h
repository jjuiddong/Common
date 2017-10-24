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
			, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM // or DXGI_FORMAT_R32_FLOAT (ShadowMap)
			, const bool isMultiSampling = true
			, const bool isDepthStencil = true
			, const DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
			, const DXGI_FORMAT depthSRVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
			, const DXGI_FORMAT depthDSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
		);

		void SetRenderTarget(cRenderer &renderer);
		void RecoveryRenderTarget(cRenderer &renderer);
		bool Begin(cRenderer &renderer
			, const Vector4 &color = Vector4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.0f)
			, const bool isClear = true );
		bool BeginSimple(cRenderer &renderer
			, const Vector4 &color = Vector4(50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.0f)
			, const bool isClear = true);
		void End(cRenderer &renderer);
		void Bind(cRenderer &renderer, const int stage = 0);
		void Clear();


	public:
		cViewport m_viewPort;
		DXGI_FORMAT m_rtvFormat;
		ID3D11Texture2D *m_texture;
		ID3D11ShaderResourceView *m_texSRV;
		ID3D11RenderTargetView *m_texRTV;
		ID3D11DepthStencilView *m_depthDSV;
		ID3D11ShaderResourceView *m_depthSRV;

		// MultiSampling Texture
		ID3D11Texture2D *m_resolvedTex;
		ID3D11ShaderResourceView *m_resolvedSRV;
	};

}