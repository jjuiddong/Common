//
// 2017-10-24, jjuiddong
//	- CubeTexture Skybox
//
#pragma once


namespace graphic
{

	class cSkyBoxCube : public cSkyBox
	{
	public:
		cSkyBoxCube();
		virtual ~cSkyBoxCube();

		bool Create(cRenderer &renderer, const char *textureFileName);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		virtual Matrix44 GetReflectMatrix();


	protected:
		bool CreateVertexBuffer(cRenderer &renderer);


	public:
		cTexture *m_texture; // reference
		cVertexBuffer m_vtxBuff;
		bool m_isDepthNone; //default: false
	};

}
