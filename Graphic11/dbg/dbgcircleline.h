//
// 2023-11-01, jjuiddong
// Debug Circle Line
//  - X-Z plane circle line
//
#pragma once


namespace graphic
{
	using namespace common;
	class cRenderer;

	class cDbgCircleLine
	{
	public:
		cDbgCircleLine();
		cDbgCircleLine(cRenderer &renderer, const float radius, const int slice
			, const cColor &color=cColor::BLACK);

		bool Create(cRenderer& renderer, const float radius, const int slice
			, const cColor& color = cColor::BLACK);
		void Render(cRenderer &renderer, const XMMATRIX &tm=XMIdentity);
		void RenderInstancing(cRenderer &renderer
			, const int count
			, const XMMATRIX *transforms
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1
		);
		void SetPos(const Vector3& pos);
		void SetRadius(const float radius);
		void SetRotation(const Quaternion& rot);
		void SetColor(const cColor& color);


	public:
		float m_radius;
		Transform m_transform;
		cDbgLineList m_lines;
	};

}
