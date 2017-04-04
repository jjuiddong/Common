// RenderTarget 전용 서피스.
// cSurface 클래스 기능과 비슷하지만, 좀더 심플한 기능을 가졌다.
// 그림자 전용으로 만들어진 클래스다.
#pragma once


namespace graphic
{

	class cSurface2
	{
	public:
		cSurface2();
		virtual ~cSurface2();

		bool Create(cRenderer &renderer, 
			//const cViewport &viewPort, 
			const int width, const int height, int mipLevels,
			D3DFORMAT texFormat= D3DFMT_X8R8G8B8, bool useDepthBuffer=true,
			D3DFORMAT depthFormat= D3DFMT_D24S8, bool autoGenMips=true,
			HANDLE *handle=NULL, const float minZ=1.f, const float maxZ=10000.f);

		void Begin();
		void End();
		void Render(cRenderer &renderer, const int index = 1);
		void RenderFull(cRenderer &renderer);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();

		bool IsLoaded() const;
		IDirect3DTexture9* GetTexture();
		const IDirect3DTexture9* GetTexture() const;


	public:
		IDirect3DTexture9 *m_texture;
		ID3DXRenderToSurface *m_rts;
		IDirect3DSurface9 *m_surface;

		int m_mipLevels;
		D3DFORMAT m_texFormat;
		bool m_useDepthBuffer;
		D3DFORMAT m_depthFormat;
		int m_width;
		int m_height;
		D3DVIEWPORT9 m_vp;
		bool m_autoGenMips;
	};


	inline bool cSurface2::IsLoaded() const { return m_texture? true : false; }
	inline IDirect3DTexture9* cSurface2::GetTexture() { return m_texture; }
	inline const IDirect3DTexture9* cSurface2::GetTexture() const { return m_texture; }
}
