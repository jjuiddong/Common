//
// 2017-08-04, jjuiddong
// DX11 Circle
//
// - 2019-09-15, jjuiddong
//	- circle plane
// 1. X-Y Plane Quad
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
//
// 2. X-Z Plane Quad
//
//     Z
//     |
//     |
//     |
//     |
//     |
//     |
//  ---|--------------------- X
//     |
#pragma once


namespace graphic
{
	using namespace common;

	class cCircleShape : public cShape
	{
	public:
		enum class ePlaneType { XY, XZ };

		cCircleShape();
		virtual ~cCircleShape();

		bool Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice
			, const int vtxType = (eVertexType::POSITION | eVertexType::COLOR)
			, const cColor &color = cColor::BLACK
			, const ePlaneType &planeType = ePlaneType::XY
		);

		void Render(cRenderer &renderer) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
	};

}
