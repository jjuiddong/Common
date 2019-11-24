//
// 2019-11-23, jjuiddong
// Equirectangular Sphere
//
#pragma once


namespace graphic
{
	using namespace common;

	class cSphere2 : public cNode
	{
	public:
		cSphere2();
		cSphere2(cRenderer &renderer, const float radius
			, const int stacks
			, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL)
			, const cColor &color = cColor::WHITE);
		virtual ~cSphere2();

		void Create(cRenderer &renderer, const float radius
			, const int stacks
			, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL)
			, const cColor &color = cColor::WHITE);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		Vector2 GetDirectionUV(const Ray &ray) const;
		float GetRadius() const;


	public:
		cSphereShape m_shape;
		cMaterial m_mtrl;
		cTexture *m_texture; // reference
		float m_radius;
	};


	inline float cSphere2::GetRadius() const { return m_radius; }
}
