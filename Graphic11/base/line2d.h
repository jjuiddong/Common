//
// 2017-11-06, jjuiddong
// 2D Line Render
//
#pragma once


namespace graphic
{
	using namespace common;

	class cLine2D : public cNode
	{
	public:
		cLine2D();
		cLine2D(cRenderer &renderer
			, const Vector2 &p0 = Vector2(0, 0)
			, const Vector2 &p1 = Vector2(100, 100)
			, const float width = 1.f
			, const int vtxType = (eVertexType::POSITION_RHW)
			, const cColor color = cColor::BLACK);

		bool Create(cRenderer &renderer
			, const Vector2 &p0 = Vector2(0, 0)
			, const Vector2 &p1 = Vector2(100, 100)
			, const float width = 1.f
			, const int vtxType = (eVertexType::POSITION_RHW)
			, const cColor color = cColor::BLACK);

		void SetLine(const Vector2 &p0, const Vector2 &p1, const float width);
		void SetColor(const cColor &color);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;


	public:
		cCubeShape m_shape;
		cColor m_color;
	};

}
