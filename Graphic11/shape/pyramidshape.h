//
// 2017-06-30, jjuiddong
// Pyramid
// Vertex = Pos + Diffuse
//
//			(head)
//            0
//           /  \
//          /    \
//       1,2 ----- 3,4
//
//       1 ----------- 3
//       | \           |
//       |    \        |
//       |      +      |
//       |       \     |
//       |          \  |
//       2 ----------- 4
//
// 2017-08-04
//	- Upgrade DX9 - DX11
//
#pragma once


namespace graphic
{
	using namespace common;

	class cPyramidShape : public cShape
	{
	public:
		cPyramidShape();
		virtual ~cPyramidShape();

		bool Create(cRenderer &renderer
			, const float width = 1
			, const float height = 1
			, const Vector3 &pos = Vector3(0, 0, 0)
			, const int vtxType = (eVertexType::POSITION | eVertexType::COLOR)
			, const cColor &color = cColor::BLACK);

		virtual void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};

}
