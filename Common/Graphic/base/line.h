//
// 정육면체 메쉬로해서 라인을 그린다.
// Vertex + Diffuse
//
#pragma once


namespace graphic
{

	class cLine
	{
	public:
		cLine();
		cLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width);

		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void SetLine(cRenderer &renderer, const Vector3 &p0, const Vector3 &p1, const float width, const D3DCOLOR color = 0);
		void SetTransform(const Matrix44 &tm);
		const Matrix44& GetTransform();


	protected:
		void InitCube(cRenderer &renderer, const D3DCOLOR color = 0);


	private:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_p0;
		Vector3 m_p1;
		float m_width;
		Matrix44 m_tm;
	};


	inline void cLine::SetTransform(const Matrix44 &tm) { m_tm = tm;  }
	inline const Matrix44& cLine::GetTransform() { return m_tm; }
}
