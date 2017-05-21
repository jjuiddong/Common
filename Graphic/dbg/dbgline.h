//
// 2017-05-13, jjuiddong
// 정육면체 메쉬로해서 라인을 그린다.
// Vertex + Diffuse
//
#pragma once


namespace graphic
{

	class cDbgLine
	{
	public:
		cDbgLine();
		cDbgLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width);

		void SetLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width, const D3DCOLOR color = 0);
		void InitCube(cRenderer &renderer, const D3DCOLOR color = 0);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_p0;
		Vector3 m_p1;
		float m_width;
		Matrix44 m_tm;
	};

}
