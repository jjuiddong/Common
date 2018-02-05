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
			, const bool isMultiSampling = true
			, const DXGI_FORMAT texFormat = DXGI_FORMAT_R32_TYPELESS
			, const DXGI_FORMAT SRVFormat = DXGI_FORMAT_R32_FLOAT
			, const DXGI_FORMAT DSVFormat = DXGI_FORMAT_D32_FLOAT
		);

		bool Begin(cRenderer &renderer, const bool isClear = true, const Vector4 &clearColor=Vector4(1,1,1,1));
		void End(cRenderer &renderer);
		void SetRenderTarget(cRenderer &renderer);
		void RecoveryRenderTarget(cRenderer &renderer);
		void Bind(cRenderer &renderer, const int stage = 0);
		void DebugRender(cRenderer &renderer);
		void Clear();


	public:
		cViewport m_viewPort;
		ID3D11Texture2D *m_texture;
		ID3D11ShaderResourceView *m_depthSRV;
		ID3D11DepthStencilView *m_depthDSV;
		cQuad2D m_quad; // Debugging
	};

}
