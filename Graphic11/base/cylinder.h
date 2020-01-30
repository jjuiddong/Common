//
// 2020-01-20, jjuiddong
// cylinder renderer
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCylinder : public cNode
	{
	public:
		cCylinder();
		cCylinder(cRenderer &renderer, const float radius, const float height
			, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE);
		virtual ~cCylinder();

		bool Create(cRenderer &renderer, const float radius, const float height
			, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		void SetColor(const cColor &color);
		void SetPos(const Vector3 &pos);
		void SetDimension(const float radius, const float height);


	public:
		cCylinderShape m_shape;
		cMaterial m_mtrl;
		float m_radius;
		float m_height;
	};

}
