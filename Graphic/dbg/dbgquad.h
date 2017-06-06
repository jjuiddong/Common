//
// 2017-06-06, jjuiddong
// Debugging, Display Quad Line
// Using VertexBuffer, LineStrip Function
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

	class cDbgQuad
	{
	public:
		cDbgQuad();
		virtual ~cDbgQuad();

		bool Create(cRenderer &renderer);
		void SetQuad(Vector3 vertices[4]);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);


	public:
		cVertexBuffer m_vtxBuff;
	};

}
