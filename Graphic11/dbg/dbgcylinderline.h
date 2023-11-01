//
// 2023-11-01, jjuiddong
// Debug Cylinder Line
//
#pragma once


namespace graphic
{
	using namespace common;
	class cRenderer;

	class cDbgCylinderLine
	{
	public:
		cDbgCylinderLine();
		cDbgCylinderLine(cRenderer &renderer, const float radius
			, const float height, const int slice
			, const cColor &color=cColor::BLACK);

		bool Create(cRenderer& renderer, const float radius, const float height
			, const int slice, const cColor& color = cColor::BLACK);
		void Render(cRenderer &renderer, const XMMATRIX &tm=XMIdentity);
		void RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
		);
		void SetPos(const Vector3& pos);
		void SetCylinder(const float radius, const float height);
		void SetRotation(const Quaternion& rot);
		void SetColor(const cColor& color);


	public:
		float m_radius;
		float m_height;
		Transform m_transform;
		cDbgLineList m_lines; // cylinder side line
		cDbgCircleLine m_circle; // top, bottom circle line
	};

}
