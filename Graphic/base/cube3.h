// 정육면체를 표현하는 클래스.
// 조명 효과를 넣을 수 있는 모델이다.
// Vertext + Normal + Texture UV 로 표현된다.
// VertexBuffer + IndexBuffer
#pragma once


namespace graphic
{

	class cRenderer;

	class cCube3
	{
	public:
		cCube3();
		cCube3(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax);

		void InitCube(cRenderer &renderer);
		void SetCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax);
		void SetCube(cRenderer &renderer, const cCube3 &cube);
		const float Length() const; // length(min - max)
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm = Matrix44::Identity);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_min;
		Vector3 m_max;
		Matrix44 m_tm;
		cMaterial m_mtrl;
		cTexture *m_tex;// reference
	};


	inline const float cCube3::Length() const { return (m_min - m_max).Length(); }
}
