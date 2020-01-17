//
// 2020-01-16, jjuiddong
// Capsule Shape
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCapsuleShape : public cShape
	{
	public:
		cCapsuleShape();
		virtual ~cCapsuleShape();

		bool Create(cRenderer &renderer, const float radius
			, const float halfHeight
			, const int stacks
			, const int slices
			, const int vtxType = (eVertexType::POSITION
				| eVertexType::NORMAL
				| eVertexType::COLOR)
			, const cColor &color = cColor::BLACK
			);

		void Render(cRenderer &renderer) override;


	protected:
		void GenerateSphereMesh(int slices, int stacks, Vector3* positions, WORD* indices);
		void GenerateConeMesh(int slices, Vector3* positions, WORD* indices, int offset);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		float m_radius0;
		float m_radius1;
	};

}
