//
// 2017-08-04, jjuiddong
// DX11 Circle
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCircleShape : public cShape
	{
	public:
		cCircleShape();
		virtual ~cCircleShape();

		bool Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice
			, const cColor &color = cColor::BLACK);

		void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};

}
