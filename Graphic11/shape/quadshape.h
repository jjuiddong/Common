//
// 2017-08-07, jjuiddong
// DX11 Quad Shape
//
// 1. X-Y Plane Quad
//
//     Y
//     |
//     |
//     |
//     |
//     |
//     |
//  ---|--------------------- X
//     |
//
//
//
// 2. X-Z Plane Quad
//
//     Z
//     |
//     |
//     |
//     |
//     |
//     |
//  ---|--------------------- X
//     |

//
#pragma once


namespace graphic
{
	using namespace common;

	class cQuadShape : public cShape
	{
	public:
		enum class ePlaneType {XY, XZ};

		cQuadShape();
		cQuadShape(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE0)
			, const cColor &color = cColor::WHITE
			, const ePlaneType planeType = ePlaneType::XY);

		virtual ~cQuadShape();

		bool Create(cRenderer &renderer
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE0)
			, const cColor &color = cColor::WHITE
			, const float width = 2
			, const float height = 2
			, const bool isDynamic = false
			, const ePlaneType planeType = ePlaneType::XY);

		void Render(cRenderer &renderer) override;

		virtual void RenderInstancing(cRenderer& renderer, const int count) override;

		void SetUV(cRenderer &renderer, const Vector2 &lt, const Vector2 &rt, const Vector2 &lb, const Vector2 &rb);


	public:
		float m_width;
		float m_height;
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cVertexLayout m_vtxLayout;
	};

}
