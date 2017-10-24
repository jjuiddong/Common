//
// 2017-08-11, jjuiddong
//	- Upgrade DX11 Skybox
//
#pragma once


namespace graphic
{

	class cSkyBox : public cNode
	{
	public:
		cSkyBox();
		virtual ~cSkyBox();

		bool Create(cRenderer &renderer, const StrPath &textureFilePath);
		bool Create(cRenderer &renderer, const StrPath &textureFilePath,
			char *skyboxTextureNames[6]);
		bool Create2(cRenderer &renderer, const StrPath &textureFilePath);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		virtual Matrix44 GetReflectMatrix();


	protected:
		bool CreateVertexBuffer(cRenderer &renderer);


	private:
		enum {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, MAX_FACE};
		cVertexBuffer m_vtxBuff;
		cTexture *m_textures[ MAX_FACE];
	};

}
