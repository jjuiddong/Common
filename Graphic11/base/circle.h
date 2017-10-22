//
// 2017-08-04, jjuiddong
// Upgrade DX11
// Circle
//
#pragma once


namespace graphic
{

	class cCircle : public cNode
	{
	public:
		cCircle();
		virtual ~cCircle();

		bool Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice
			, const cColor &color = cColor::BLACK);

		void Render(cRenderer &renderer);


	public:
		float m_radius;
		cCircleShape m_shape;
	};

}
