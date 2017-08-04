//
// 2017-08-04, jjuiddong
// Upgrade DX11
// Sphere Shape
//
#pragma once


namespace graphic
{

	class cSphereShape : public cShape
	{
	public:
		cSphereShape();
		virtual ~cSphereShape();

		bool Create(cRenderer &renderer, const float radius, const int stacks, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE)
			, const cColor &color = cColor::BLACK);

		void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		int m_numSlices;
		int m_numStacks;
	};

}
