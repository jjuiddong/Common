//
// 2023-01-21, jjuiddong
// capsule renderer
//	- using capsule shape
//	- halfsphere + cylinder shape
//	- p0+p1 position setup
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCapsule2 : public cNode
	{
	public:
		cCapsule2();
		cCapsule2(cRenderer &renderer, const float radius, const float halfHeight
			, const int stacks, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE);
		virtual ~cCapsule2();

		bool Create(cRenderer &renderer, const float radius, const float halfHeight
			, const int stacks, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		void SetColor(const cColor &color);
		void SetPos(const Vector3 &pos);
		void SetDimension(const float radius, const float halfHeight);
		void SetCapsule(const Vector3& p0, const Vector3& p1, const float radius);


	public:
		cCapsuleShape m_shape;
		cMaterial m_mtrl;
		float m_radius;
		float m_halfHeight;
	};

}
