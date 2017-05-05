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

		bool Create(cRenderer &renderer, const string &fileName);
		void Bind(cShader &shader, const string &key);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		bool IsLoaded();
		void Clear();


		string m_fileName;
		IDirect3DCubeTexture9 *m_texture;
	};

}
