//
// 2017-10-02, jjuiddong
// Torus Shape
// Lay on X-Z Plane
//
#pragma once


namespace graphic
{
	using namespace common;

	class cTorusShape : public cShape
	{
	public:
		cTorusShape();
		virtual ~cTorusShape();

		bool Create(cRenderer &renderer, const float outerRadius, const float innerRadius
			, const int stack = 10, const int slice=10
			, const int vtxType=(eVertexType::POSITION));

		virtual void Render(cRenderer &renderer) override;

		void Clear();


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};

}
