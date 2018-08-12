//
// 2018-08-12, jjuiddong
// 3d texture
//
#pragma once


namespace graphic
{
	using namespace common;

	class cTexture3d
	{
	public:
		cTexture3d();
		cTexture3d(ID3D11ShaderResourceView *srv);
		virtual ~cTexture3d();

		bool Create(cRenderer &renderer, const StrPath &fileName);

		bool Create(cRenderer &renderer, const int width, const int height
			, const DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM
			, const D3D11_USAGE usage = D3D11_USAGE_DYNAMIC
		);

		bool Create(cRenderer &renderer, const int width, const int height
			, const DXGI_FORMAT format
			, const void *pMem
			, const int pitchLength
			, const D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		bool WritePNGFile(cRenderer &renderer, const StrPath &fileName);

		void Bind(cRenderer &renderer, const int stage = 0);
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
		ID3D11Texture3D *m_texture;
		ID3D11ShaderResourceView *m_texSRV;
	};


	inline const StrPath& cTexture3d::GetTextureName() const { return m_fileName; }
}
