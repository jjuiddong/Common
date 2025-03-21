//
// 2020-01-20, jjuiddong
// Cylinder Shape
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCylinderShape : public cShape
	{
	public:
		cCylinderShape();
		virtual ~cCylinderShape();

		bool Create(cRenderer &renderer, const float radius
			, const float height
			, const int slices
			, const int vtxType = (eVertexType::POSITION
				| eVertexType::NORMAL
				| eVertexType::COLOR)
			, const cColor &color = cColor::BLACK
			, const bool isHead = true
		);

		void Render(cRenderer &renderer) override;

		void GenerateConeMesh(int slices, Vector3* positions, WORD* indices, int offset
			, const bool isHead = true);

		void GenerateConeMesh2(const int slices, const float radius, const float height
			, Vector3* positions, WORD* indices, int offset
			, const bool isHead = true);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		float m_radius;
		float m_height;
	};

}
