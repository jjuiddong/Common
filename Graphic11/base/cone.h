//
// 2020-02-10, jjuiddong
// Cone Renderer
//
#pragma once


namespace graphic
{

	class cCone : public cNode
	{
	public:
		cCone();
		virtual ~cCone();

		bool Create(cRenderer &renderer, const float radius, const float height
			, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE);

		virtual bool Render(cRenderer &renderer
			, const XMMATRIX &parentTm = XMIdentity
			, const int flags = 1) override;

		void SetPos(const Vector3 &pos);
		void SetDimension(const float radius, const float height);
		void SetRadius(const float radius);
		void SetRadiusXZ(const float radiusX, const float radiusZ);
		void SetHeight(const float height);


	public:
		float m_radiusX;
		float m_radiusZ;
		float m_height;
		cColor m_color;
		cConeShape m_shape;
	};

}

