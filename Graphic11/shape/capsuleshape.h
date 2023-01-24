//
// 2023-01-21, jjuiddong
// Capsule Shape
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCapsuleShape : public cShape
	{
	public:
		cCapsuleShape();
		virtual ~cCapsuleShape();

		bool Create(cRenderer &renderer
			, const float radius
			, const float height
			, const int stacks
			, const int slices
			, const int vtxType = (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
			, const cColor &color = cColor::BLACK
			);

		void Render(cRenderer &renderer) override;
		void Render(cRenderer& renderer, const float radius, const float height
			, const XMMATRIX& parentTm = XMIdentity);


	protected:
		std::pair<int,int> GenerateHalfSphereMesh( BYTE* vertices, WORD* indices
			, const int startVerticesIdx, const int startIndicesIdx
			, const int vtxType, const float radius, const int stacks, const int slices
			, const cColor &color, const bool isFlipY, const float yOffset);


	public:
		cVertexBuffer m_vtxBuff1; // cylinder
		cIndexBuffer m_idxBuff1;
		cVertexBuffer m_vtxBuff2; // half sphere
		cIndexBuffer m_idxBuff2;
		int m_stacks;
		int m_slices;
	};

}
