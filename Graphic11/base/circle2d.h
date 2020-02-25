//
// 2019-07-12, jjuiddong
// 2D Circle
//
// X-Y Plane Circle
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCircle2D : public cNode
	{
	public:
		cCircle2D();
		virtual ~cCircle2D();

		bool Create(cRenderer &renderer, const Vector3 &center
			, const float radius, const int slice
			, const cColor &color = cColor::BLACK);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1) override;

		bool Render2(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1);

		void SetPos(const Vector2 &pos);


	public:
		float m_radius;
		cColor m_color;
		cCircleShape m_shape;
	};

}
