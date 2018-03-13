//
// 2017-05-13, jjuiddong
// 정육면체 메쉬로해서 라인을 그린다.
// Vertex = Point + Diffuse
//
// 2017-08-07
//	- Upgrade DX11
//
#pragma once


namespace graphic
{
	using namespace common;

	class cDbgLine
	{
	public:
		cDbgLine();
		cDbgLine(cRenderer &renderer
			, const Vector3 &p0 = Vector3(0, 0, 0)
			, const Vector3 &p1 = Vector3(1, 1, 1)
			, const float width = 1.f
			, const cColor color = cColor::BLACK);

		bool Create(cRenderer &renderer
			, const Vector3 &p0 = Vector3(0, 0, 0)
			, const Vector3 &p1 = Vector3(1, 1, 1)
			, const float width = 1.f
			, const cColor color = cColor::BLACK);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1);

		void SetLine(const Vector3 &p0, const Vector3 &p1, const float width);
		void SetColor(const cColor &color);


	public:
		bool m_isSolid;
		Transform m_transform;
		cCubeShape m_shape;
		cColor m_color;
	};

}
