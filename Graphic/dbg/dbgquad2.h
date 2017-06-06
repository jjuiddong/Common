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
#pragma once

namespace graphic
{

	class cDbgQuad2
	{
	public:
		cDbgQuad2();
		virtual ~cDbgQuad2();

		bool Create(cRenderer &renderer);
		void SetQuad(Vector3 vertices[4], const float width=1.f);
		void SetColor(const DWORD color);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);


	public:
		Vector3 m_pos[4];
		cLine m_lines[4];
	};

}

