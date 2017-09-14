//
// 2017-07-31, jjuiddong
// Upgrade DX9 - DX11 
// Texture Class
// IDirect3DTexture9 인터페이스를 쉽게 사용하기위해 만들어짐.
//
// 2017-07-24
//	- TextDesigner
//	- https://www.codeproject.com/Articles/42529/Outline-Text
//
// 2017-08-25
//	- Upgrade DX11, wchar_t
//
#pragma once


namespace graphic
{

	class cTexture
	{
	public:
		cTexture();
		cTexture(ID3D11ShaderResourceView *srv);
		virtual ~cTexture();

		bool Create(cRenderer &renderer, const StrPath &fileName);
		bool Create(cRenderer &renderer, const int width, const int height
			, const DXGI_FORMAT format= DXGI_FORMAT_R8G8B8A8_UNORM);
		bool WritePNGFile( const StrPath &fileName );

		void Bind(cRenderer &renderer, const int stage=0);
		void Bind(cShader &shader, const Str32 &key);
		void Unbind(cRenderer &renderer, const int stage);
		void Render2D(cRenderer &renderer);
		void CopyFrom(cTexture &src);
		//void CopyFrom(IDirect3DTexture9 *src);
		void* Lock(cRenderer &renderer);
		void Unlock(cRenderer &renderer);
		const StrPath& GetTextureName() const;
		bool DrawText(cRenderer &renderer, const wchar_t *text
			, const cColor &color, const cColor &outlineColor, Vector2 &textSize);
		bool IsLoaded();
		int Width();
		int Height();
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();
		
		static StrPath ConvertTextureFileName(const char *fileName);


	public:
		ID3D11ShaderResourceView *m_texture;
		ID3D11Texture2D *m_rawTex;
		D3D11_TEXTURE2D_DESC m_imageInfo;
		StrPath m_fileName;
		bool m_isReferenceMode;
		bool m_customTexture; // using LostDevice, ResetDevice
	};


	inline const StrPath& cTexture::GetTextureName() const { return m_fileName; }
}
