//
// 2017-07-05, jjuiddong
// Quad 2D
//	- Simple 2d Rendering Quad class
//	- Independent from Camera
//
// 2017-09-06
//	- Upgrade DX11
//
// 2017-11-06
//	- Collision Check
//	- m_transform.pos : Screen X,Y Pos
//	- m_transform.scale : Width, Height
//
#pragma once


namespace graphic
{
	using namespace common;

	class cQuad2D : public cNode
	{
	public:
		cQuad2D();
		virtual ~cQuad2D();

		bool Create(cRenderer &renderer, const float x, const float y, const float width, const float height
			, const int vtxType = (eVertexType::POSITION_RHW | eVertexType::COLOR | eVertexType::TEXTURE0)
			, const char *textureFileName = NULL
			, const Vector2 *uvs = NULL);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		void SetPosition(const float x, const float y
			, const float width, const float height );

		virtual cNode* Picking(const Ray &ray, const eNodeType::Enum type
			, const XMMATRIX &parentTm
			, const bool isSpherePicking = true
			, OUT float *dist = NULL) override;


	public:
		cQuadShape m_shape;
		cColor m_color;
		cTexture *m_texture; // reference
		bool m_isDepthNone; // default true
	};

}
