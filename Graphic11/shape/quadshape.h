//
// 2017-08-07, jjuiddong
// DX11 Quad Shape
// X-Y Plane Quad
//
//     Y
//     |
//     |
//     |
//     |
//     |
//     |
//  ---|--------------------- X
//     |
//
//
#pragma once


namespace graphic
{
	using namespace common;

	class cQuadShape : public cShape
	{
	public:
		cQuadShape();
		cQuadShape(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE0)
			, const cColor &color = cColor::WHITE);

		virtual ~cQuadShape();

		bool Create(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE0)
			, const cColor &color = cColor::WHITE
			, const float width = 2
			, const float height = 2
			, const bool isDynamic = false);

		void Render(cRenderer &renderer) override;

		void SetUV(cRenderer &renderer, const Vector2 &lt, const Vector2 &rt, const Vector2 &lb, const Vector2 &rb);


	public:
		float m_width;
		float m_height;
		cVertexBuffer m_vtxBuff;
		cVertexLayout m_vtxLayout;
	};

}
