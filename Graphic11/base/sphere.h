//
// 2016-05-22, jjuiddong
// 구 를 출력하는 클래스.
// 
// 2016-05-22
//		- cNode 클래스 상속받아서 일반화 시킴
//
// 2017-08-04
//		- Upgrade DX9 - DX11
//
#pragma once


namespace graphic
{
	using namespace common;

	class cSphere : public cNode
	{
	public:
		cSphere();
		cSphere(cRenderer &renderer, const float radius, const int stacks, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL)
			, const cColor &color = cColor::WHITE);
		virtual ~cSphere();

		void Create(cRenderer &renderer, const float radius, const int stacks, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL)
			, const cColor &color = cColor::WHITE);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		float GetRadius() const;


	public:
		cSphereShape m_shape;
		cMaterial m_mtrl;
		cTexture *m_texture; // reference
		float m_radius;
	};	


	inline float cSphere::GetRadius() const { return m_radius; }
}
