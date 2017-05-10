//
// 2017-05-09, jjuiddong
// ShadowMap Surface
//
#pragma once


namespace graphic
{

	class cSurface3
	{
	public:
		cSurface3();
		virtual ~cSurface3();

		bool Create(cRenderer &renderer,
			//const cViewport &viewPort, 
			const int width, const int height, int mipLevels,
			D3DFORMAT texFormat = D3DFMT_X8R8G8B8, bool useDepthBuffer = true,
			D3DFORMAT depthFormat = D3DFMT_D24S8, bool autoGenMips = true,
			HANDLE *handle = NULL, const float minZ = 1.f, const float maxZ = 10000.f
			, const D3DMULTISAMPLE_TYPE sampleType = D3DMULTISAMPLE_NONE);

		void Begin(cRenderer &renderer);
		void End(cRenderer &renderer);
		void Render(cRenderer &renderer, const int index = 1);
		void RenderFull(cRenderer &renderer);
		void Bind(cShader &shader, const string &key);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();

		bool IsLoaded() const;
		IDirect3DTexture9* GetTexture();
		const IDirect3DTexture9* GetTexture() const;


	public:
		IDirect3DTexture9 *m_texture;
		IDirect3DSurface9 *m_pDSShadow;
		//ID3DXRenderToSurface *m_rts;
		//IDirect3DSurface9 *m_surface;

		IDirect3DSurface9 *m_oldRt;
		IDirect3DSurface9 *m_oldDs;


		int m_mipLevels;
		D3DFORMAT m_texFormat;
		bool m_useDepthBuffer;
		D3DFORMAT m_depthFormat;
		bool m_autoGenMips;
		D3DVIEWPORT9 m_vp;
	};


	inline bool cSurface3::IsLoaded() const { return m_texture ? true : false; }
	inline IDirect3DTexture9* cSurface3::GetTexture() { return m_texture; }
	inline const IDirect3DTexture9* cSurface3::GetTexture() const { return m_texture; }
}
