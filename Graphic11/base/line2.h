//
// 정육면체 메쉬로해서 라인을 그린다.
// Vertex + Normal + Diffuse
#pragma once


namespace graphic
{

	class cLine2
	{
	public:
		cLine2();
		cLine2(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width);

		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm = Matrix44::Identity);
		void SetLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width);
		cMaterial& GetMaterial();
		void SetTransform(const Matrix44 &tm);
		const Matrix44& GetTransform();
		void SetColor(DWORD color);


	protected:
		void InitCube(cRenderer &renderer);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cMaterial m_material;
		Vector3 m_p0;
		Vector3 m_p1;
		float m_width;
		Matrix44 m_tm;
	};


	inline cMaterial& cLine2::GetMaterial() { return m_material; }
	inline void cLine2::SetTransform(const Matrix44 &tm) { m_tm = tm; }
	inline const Matrix44& cLine2::GetTransform() { return m_tm; }
}
