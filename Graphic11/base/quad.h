//
// 2017-05-06, jjuiddong
// 사각형을 나타내는 클래스다.
// 간단하게 텍스쳐를 입힐 때 사용할 수 있는 클래스.
// Vertex = Point + UV
//
// 2017-08-07
//	- Upgrade DX11
//
#pragma once


namespace graphic
{
	using namespace common;

	class cQuad : public cNode
	{
	public:
		cQuad();
		cQuad(cRenderer &renderer, const float width, const float height, const Vector3 &pos
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE0)
			, const StrPath &textureFileName = ""
			, const cQuadShape::ePlaneType planeType = cQuadShape::ePlaneType::XY);
		virtual ~cQuad();

		bool Create(cRenderer &renderer, const float width, const float height, const Vector3 &pos
			, const int vtxType = (eVertexType::POSITION | eVertexType::TEXTURE0)
			, const StrPath &textureFileName = ""
			, const bool isDynamic = false
			, const cQuadShape::ePlaneType planeType = cQuadShape::ePlaneType::XY);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;

		virtual bool RenderInstancing(cRenderer& renderer
			, const int count
			, const Matrix44* transforms
			, const XMMATRIX& parentTm = XMIdentity
			, const int flags = 1
		) override;

		virtual bool RenderInstancing2(cRenderer& renderer
			, const int count
			, const Matrix44* transforms
			, const Vector4* colors
			, const XMMATRIX& parentTm = XMIdentity
			, const int flags = 1
		);

		//void SetUV(cRenderer &renderer, const Vector2 &lt, const Vector2 &rt, const Vector2 &lb, const Vector2 &rb);


	public:
		cQuadShape m_shape;
		cColor m_color;
		cTexture *m_texture; // reference
	};

}
