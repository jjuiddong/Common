// 텍스쳐 클래스
// IDirect3DTexture9 인터페이스를 쉽게 사용하기위해 만들어짐.
#pragma once


namespace graphic
{

	class cTexture
	{
	public:
		cTexture();
		virtual ~cTexture();

		bool Create(cRenderer &renderer, const string &fileName, bool isSizePow2 = true);
		bool Create(cRenderer &renderer, const int width, const int height, const D3DFORMAT format);
		bool WritePNGFile( const string &fileName );

		void Bind(cRenderer &renderer, const int stage);
		void Bind(cShader &shader, const string &key);
		void Unbind(cRenderer &renderer, const int stage);
		void Render2D(cRenderer &renderer);
		void CopyFrom(cTexture &src);
		void CopyFrom(IDirect3DTexture9 *src);
		bool Lock(D3DLOCKED_RECT &out);
		void Unlock();
		IDirect3DTexture9* GetTexture();
		const D3DXIMAGE_INFO& GetImageInfo() const;
		const string& GetTextureName() const;
		void TextOut(cFontGdi &font, const string &text, const int x, const int y, const DWORD color);
		void DrawText(cFontGdi &font, const string &text, const sRect &rect, const DWORD color);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();
		

	protected:
		bool CreateEx(cRenderer &renderer, const string &fileName);


	public:
		IDirect3DTexture9 *m_texture;
		D3DXIMAGE_INFO m_imageInfo;
		string m_fileName;
		bool m_isReferenceMode;
	};


	inline IDirect3DTexture9* cTexture::GetTexture() { return m_texture; }
	inline const D3DXIMAGE_INFO& cTexture::GetImageInfo() const { return m_imageInfo; }
	inline const string& cTexture::GetTextureName() const { return m_fileName; }
}
