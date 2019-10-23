//
// 2017-08-04, jjuiddong
// Upgrade DX11
// Circle
//
// X-Y Plane Circle
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCircle : public cNode
	{
	public:
		cCircle();
		virtual ~cCircle();

		bool Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice
			, const cColor &color = cColor::BLACK
			, const cCircleShape::ePlaneType planeType = cCircleShape::ePlaneType::XY
		);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		void SetPos(const Vector3 &pos);


	public:
		float m_radius;
		cColor m_color;
		cCircleShape m_shape;
	};

}
