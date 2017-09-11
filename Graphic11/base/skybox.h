//
// 2017-08-11, jjuiddong
//	- Upgrade DX11 Skybox
//
#pragma once


namespace graphic
{

	class cSkyBox : public cNode2
	{
	public:
		cSkyBox();
		virtual ~cSkyBox();

		bool Create(cRenderer &renderer, const StrPath &textureFilePath);
		bool Create(cRenderer &renderer, const StrPath &textureFilePath,
			char *skyboxTextureNames[6]);
		bool Create2(cRenderer &renderer, const StrPath &textureFilePath);
		void Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);


	protected:
		bool CreateVertexBuffer(cRenderer &renderer);


	private:
		enum {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, MAX_FACE};
		cVertexBuffer m_vtxBuff;
		cTexture *m_textures[ MAX_FACE];
	};

}
