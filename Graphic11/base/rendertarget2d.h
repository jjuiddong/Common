//
// 2018-08-12, jjuiddong
// DX11 Simple RenderTarget
//	- 2d texture + render target + shader resource
//
#pragma once


namespace graphic
{
	using namespace common;

	class cRenderTarget2d
	{
	public:
		cRenderTarget2d();
		virtual ~cRenderTarget2d();

		bool Create(cRenderer &renderer
			, const int width, const int height
			, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM // or DXGI_FORMAT_R32_FLOAT (ShadowMap)
		);

		void Bind(cRenderer &renderer, const int stage = 0);
		void Unbind(cRenderer &renderer);
		void Clear();


	public:
		DXGI_FORMAT m_rtvFormat;
		ID3D11Texture2D *m_texture;
		ID3D11ShaderResourceView *m_srv;
		ID3D11RenderTargetView *m_rtv;
	};

}
