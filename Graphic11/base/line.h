//
// 정육면체 메쉬로해서 라인을 그린다.
// Vertex + Diffuse
//
#pragma once


namespace graphic
{

	class cLine : public cNode2
	{
	public:
		cLine();
		cLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width);

		void Create(cRenderer &renderer);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm = Matrix44::Identity);
		void SetLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width, const cColor color=cColor::BLACK);
		void SetLine(const Vector3 &p0, const Vector3 &p1, const float width);
		void SetColor(const DWORD color);


	protected:
		void InitCube(cRenderer &renderer, const cColor color = cColor::BLACK);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_p0;
		Vector3 m_p1;
		float m_width;
		Matrix44 m_tm;
	};

}
