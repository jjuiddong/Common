//
// 2025-05-05, jjuiddong
// Render Axis with Quad
//
#pragma once


namespace graphic
{
	using namespace common;

	class cDbgAxis2
	{
	public:
		cDbgAxis2();
		virtual ~cDbgAxis2();

		bool Create(cRenderer &renderer, const float alpha = 0.5f
			, const float length = 1.f
			, const float width = 1.f
		);
		void SetAxis(const float size, const cBoundingBox &bbox, const bool approximate=true);
		void SetAxis(const cBoundingBox &bbox, const bool approximate = true);
		void Render(cRenderer &renderer, const XMMATRIX &tm=XMIdentity);


	public:
		float m_length; // line length
		float m_width; // line width
		cQuad m_quad; // axis line
	};

}
