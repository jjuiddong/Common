////
// 2017-07-31, jjuiddong
// Upgrade DX9 - DX11 
// Texture Class
// IDirect3DTexture9 인터페이스를 쉽게 사용하기위해 만들어짐.
//
// 2017-07-24
//	- TextDesigner
//	- https://www.codeproject.com/Articles/42529/Outline-Text
//
#pragma once


namespace graphic
{

	class cTexture
	{
	public:
		cTexture();
		virtual ~cTexture();

		bool Create(cRenderer &renderer, const StrPath &fileName);
		//bool Create(cRenderer &renderer, const int width, const int height, const D3DFORMAT format);
		bool WritePNGFile( const StrPath &fileName );

		void Bind(cRenderer &renderer, const int stage=0);
		void Bind(cShader &shader, const Str32 &key);
		void Unbind(cRenderer &renderer, const int stage);
		void Render2D(cRenderer &renderer);
		void CopyFrom(cTexture &src);
		//void CopyFrom(IDirect3DTexture9 *src);
		//bool Lock(D3DLOCKED_RECT &out);
		void Unlock();
		//IDirect3DTexture9* GetTexture();
		//const D3DXIMAGE_INFO& GetImageInfo() const;
		const StrPath& GetTextureName() const;
		void TextOut(const Str128 &text, const int x, const int y, const DWORD color);
		void DrawText(const Str128 &text, const sRecti &rect, const DWORD color);
		bool DrawText2(cRenderer &renderer, const Str128 &text, 
			const cColor &color, const cColor &outlineColor, Vector2 &textSize);
		bool IsLoaded();
		int Width();
		int Height();
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();
		

	public:
		//IDirect3DTexture9 *m_texture;
		ID3D11ShaderResourceView *m_texture;
		//D3DXIMAGE_INFO m_imageInfo;
		D3D11_TEXTURE2D_DESC m_imageInfo;
		StrPath m_fileName;
		bool m_isReferenceMode;
		bool m_customTexture; // using LostDevice, ResetDevice
	};


	//inline IDirect3DTexture9* cTexture::GetTexture() { return m_texture; }
	//inline const D3DXIMAGE_INFO& cTexture::GetImageInfo() const { return m_imageInfo; }
	inline const StrPath& cTexture::GetTextureName() const { return m_fileName; }
}
