//
// 2017-08-07, jjuiddong
// DX11 Quad Shape
//
#pragma once


namespace graphic
{

	class cQuadShape : public cShape
	{
	public:
		cQuadShape();
		cQuadShape(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE)
			, const cColor &color = cColor::WHITE);

		virtual ~cQuadShape();

		bool Create(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE)
			, const cColor &color = cColor::WHITE);

		void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
	};

}
