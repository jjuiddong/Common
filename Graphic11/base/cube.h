//
// 2017-07-28, jjuiddong
// DX11 Cube Class
// Vertex = Position + Normal + Diffuse + TextureUV
// VertexBuffer + IndexBuffer
//
#pragma once


namespace graphic
{

	class cRenderer;
	class cCube : public cNode
	{
	public:
		cCube();
		cCube(cRenderer &renderer, const cBoundingBox &bbox
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE
			, const int uvFlag = 1);

		virtual bool Create(cRenderer &renderer, const cBoundingBox &bbox
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE
			, const int uvFlag = 1);
		virtual bool Create(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE
			, const int uvFlag = 1);
		void SetCube(const cBoundingBox &bbox);
		void SetCube(cRenderer &renderer, const cBoundingBox &bbox);
		void SetCube(cRenderer &renderer, const cCube &cube);
		void SetColor( const cColor &color );

		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;
		virtual cNode* Clone(cRenderer &renderer) const override;


	public:
		cCubeShape m_shape;
		cTexture *m_texture; // reference
		cColor m_color;
	};	

}
