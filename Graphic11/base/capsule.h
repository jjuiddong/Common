//
// 2020-01-16, jjuiddong
// capsule renderer
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCapsule : public cNode
	{
	public:
		cCapsule();
		cCapsule(cRenderer &renderer, const float radius, const float halfHeight
			, const int stacks, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE);
		virtual ~cCapsule();

		bool Create(cRenderer &renderer, const float radius, const float halfHeight
			, const int stacks, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		void SetColor(const cColor &color);
		void SetPos(const Vector3 &pos);
		void SetDimension(const float radius, const float halfHeight);


	public:
		cSphereShape m_sphereShape;
		cCylinderShape m_cylinderShape;
		cMaterial m_mtrl;
		float m_radius;
		float m_halfHeight;
	};

}
