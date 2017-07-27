//
// 2017-05-05, jjuiddong
// cube texture
//
#pragma once


namespace graphic
{

	class cCubeTexture
	{
	public:
		cCubeTexture();
		virtual ~cCubeTexture();

		bool Create(cRenderer &renderer, const StrPath &fileName);
		void Bind(cShader &shader, const Str32 &key);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		bool IsLoaded();
		void Clear();


		StrPath m_fileName;
		IDirect3DCubeTexture9 *m_texture;
	};

}
