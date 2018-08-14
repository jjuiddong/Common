//
// 2018-08-12, jjuiddong
// DX11 Simple RenderTarget
//	- 3d texture + render target + shader resource
//
#pragma once


namespace graphic
{
	using namespace common;

	class cRenderTarget3d
	{
	public:
		cRenderTarget3d();
		virtual ~cRenderTarget3d();

		bool Create(cRenderer &renderer
			, const int width, const int height, const int depth
			, const DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM // or DXGI_FORMAT_R32_FLOAT (ShadowMap)
		);

		void Bind(cRenderer &renderer, const int stage = 0);
		void Unbind(cRenderer &renderer);
		void Clear();


	public:
		DXGI_FORMAT m_rtvFormat;
		ID3D11Texture3D *m_texture;
		ID3D11ShaderResourceView *m_srv;
		ID3D11RenderTargetView *m_rtv;
	};

}
