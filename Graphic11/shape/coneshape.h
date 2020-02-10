//
// 2020-02-10, jjuiddong
// Cone Shape
//
//          Y axis (height)
//          .
//        / | \
//      /   |   \
//    /     |     \
//   --------------- X-Z plane
//        radius
#pragma once


namespace graphic
{
	using namespace common;

	class cConeShape : public cShape
	{
	public:
		cConeShape();
		virtual ~cConeShape();

		bool Create(cRenderer &renderer, const float radius, const float height
			, const int slices
			, const int vtxType = (eVertexType::POSITION| eVertexType::NORMAL| eVertexType::COLOR)
			, const cColor &color = cColor::BLACK
		);

		void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
	};

}
