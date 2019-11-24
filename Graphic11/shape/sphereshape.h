//
// 2017-08-04, jjuiddong
// Upgrade DX11
// Sphere Shape
//
#pragma once


namespace graphic
{
	using namespace common;

	class cSphereShape : public cShape
	{
	public:
		cSphereShape();
		virtual ~cSphereShape();

		bool Create(cRenderer &renderer, const float radius
			, const int stacks
			, const int slices
			, const int vtxType = (eVertexType::POSITION
				| eVertexType::NORMAL
				| eVertexType::COLOR)
			, const cColor &color = cColor::BLACK
			, const bool isClockWise = true );

		void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		int m_numSlices;
		int m_numStacks;
	};

}
