//
// 2017-07-17, jjuiddong
// 정육면체를 표현하는 클래스.
// Vertex = Position + Normal + Diffuse + Texture UV 
// VertexBuffer + IndexBuffer
// Node2 Derivated
#pragma once


namespace graphic
{

	class cRenderer;

	class cCube4 : public cNode2
	{
	public:
		cCube4();
		cCube4(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax
			, const float uvFactor = 1.f);

		void InitCube(cRenderer &renderer, const float uvFactor = 1.f);
		void InitCube2(cRenderer &renderer, const float uvFactor = 1.f);
		void InitCube2(cRenderer &renderer, Vector3 vertices[8], const float uvFactor = 1.f);
		void InitCube3(cRenderer &renderer, Vector3 vertices[24], const float uvFactor = 1.f);
		void SetCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax, const float uvFactor = 1.f);
		void SetCube(const Vector3 &vMin, const Vector3 &vMax, const float uvFactor = 1.f);
		void SetCube2(const Vector3 &vMin, const Vector3 &vMax, const float uvFactor = 1.f);
		void SetCube(const cBoundingBox &bbox, const float uvFactor = 1.f);
		const float Length() const; // length(min - max)
		virtual bool Render(cRenderer &renderer, const Matrix44 &parentTm = Matrix44::Identity, const int flags = 1) override;
		void SetColor(const DWORD color);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_min;
		Vector3 m_max;
		cMaterial m_mtrl;
		float m_uvFactor;
		cTexture *m_tex;// reference
	};


	inline const float cCube4::Length() const { return (m_min - m_max).Length(); }
}

