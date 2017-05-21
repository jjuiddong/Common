//
// 2017-05-20, jjuiddong
// Debug Box 
//		- Vertext + Diffuse 로 표현된다.
//		- Line Render
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cDbgBox2
	{
	public:
		cDbgBox2();
		cDbgBox2(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax);

		void InitBox(cRenderer &renderer);
		void InitBox(cRenderer &renderer, Vector3 vertices[8]);
		void SetBox(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax);
		void SetColor(DWORD color);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_min;
		Vector3 m_max;
		Matrix44 m_tm;
	};

}
