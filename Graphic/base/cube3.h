// 정육면체를 표현하는 클래스.
// 조명 효과를 넣을 수 있는 모델이다.
// Vertex = Position + Normal + Diffuse + Texture UV
// VertexBuffer + IndexBuffer
#pragma once


namespace graphic
{

	class cRenderer;

	class cCube3 : public iShaderRenderer
	{
	public:
		cCube3();
		cCube3(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax
			, const float uvFactor=1.f);

		void InitCube(cRenderer &renderer, const float uvFactor = 1.f);
		void InitCube2(cRenderer &renderer, const float uvFactor = 1.f);
		void InitCube2(cRenderer &renderer, Vector3 vertices[8], const float uvFactor = 1.f);
		void InitCube3(cRenderer &renderer, Vector3 vertices[24], const float uvFactor = 1.f);
		void SetCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax, const float uvFactor = 1.f);
		void SetCube(const Vector3 &vMin, const Vector3 &vMax, const float uvFactor = 1.f);
		void SetCube2(const Vector3 &vMin, const Vector3 &vMax, const float uvFactor = 1.f);
		void SetCube(cRenderer &renderer, const cCube3 &cube);
		void SetCube(const cCube3 &cube);
		void SetCube(const cBoundingBox &bbox, const float uvFactor = 1.f);
		const float Length() const; // length(min - max)
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm = Matrix44::Identity);
		virtual void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		void SetColor(const DWORD color);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_scale;
		Vector3 m_pos;
		Vector3 m_min;
		Vector3 m_max;
		Matrix44 m_tm;
		cMaterial m_mtrl;
		float m_uvFactor;
		cTexture *m_tex;// reference
	};


	inline const float cCube3::Length() const { return (m_min - m_max).Length(); }
}
