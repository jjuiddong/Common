//
// 2017-07-31, jjuiddong
// Upgrade DX9 - DX11 
// Texture Class
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

		bool Create(cRenderer &renderer, const int width, const int height
			, const DXGI_FORMAT format
			, const void *pMem
			, const int pitchLength );

		bool WritePNGFile(cRenderer &renderer, const StrPath &fileName );

		void Bind(cRenderer &renderer, const int stage=0);
		void Bind(cShader &shader, const Str32 &key);
		void Unbind(cRenderer &renderer, const int stage);
		void* Lock(cRenderer &renderer, OUT D3D11_MAPPED_SUBRESOURCE &out);
		void Unlock(cRenderer &renderer);
		const StrPath& GetTextureName() const;
		bool DrawText(cRenderer &renderer, const wchar_t *text
			, const cColor &color, const cColor &outlineColor, Vector2 &textSize);
		bool IsLoaded();
		int Width();
		int Height();
		void Clear();
		
		static StrPath ConvertTextureFileName(const char *fileName);


	public:
		StrPath m_fileName;
		D3D11_TEXTURE2D_DESC m_imageInfo;
		bool m_isReferenceMode;
		ID3D11Texture2D *m_texture;
		ID3D11ShaderResourceView *m_texSRV;
	};


	inline const StrPath& cTexture::GetTextureName() const { return m_fileName; }
}
