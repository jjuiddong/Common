//
// 2019-09-15, jjuiddong
// Quad 2D
//	- Dependent from Camera
//
#pragma once


namespace graphic
{
	using namespace common;

	class cQuad2D2 : public cNode
	{
	public:
		cQuad2D2();
		virtual ~cQuad2D2();

		bool Create(cRenderer &renderer, const float x, const float y, const float width, const float height
			, const int vtxType = (eVertexType::POSITION_RHW | eVertexType::COLOR | eVertexType::TEXTURE0)
			, const char *textureFileName = NULL);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		void SetPosition(const float x, const float y
			, const float width, const float height);

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
