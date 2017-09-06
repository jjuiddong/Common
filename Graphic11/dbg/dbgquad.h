//
// 2017-06-06, jjuiddong
// Debugging, Display Quad Line
// Using cLine
//
//   0 -------------- 1
//   |                     |
//   |                     |
//   |                     |
//   3 -------------- 2
//    Vertex Index
//
// 2017-09-04
//	- Upgrade DX11
//
#pragma once

namespace graphic
{

	class cDbgQuad
	{
	public:
		cDbgQuad();
		virtual ~cDbgQuad();

		bool Create(cRenderer &renderer);
		void SetQuad(const Vector3 vertices[4], const float width = 1.f);
		void SetColor(const cColor &color);
		void Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);


	public:
		Vector3 m_pos[4];
		cLine m_lines[4];
	};

}
