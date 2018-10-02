//
// 2017-05-06, jjuiddong
// Grid Class
//
#pragma once

namespace graphic
{
	using namespace common;

	class cRenderer;
	class cGrid : public cNode
	{
	public:
		cGrid();
		virtual ~cGrid();

		void Create(cRenderer &renderer, const int rowCellCount, const int colCellCount
			, const float cellSizeW
			, const float cellSizeH
			, const int vertexType = (eVertexType::POSITION | eVertexType::COLOR)
			, const cColor &color = cColor::WHITE
			, const char *textureFileName = g_defaultTexture
			, const Vector2 &uv0 = Vector2(0, 0)
			, const Vector2 &uv1 = Vector2(1, 1)
			, const float textureUVFactor = 1.f
			, const bool isEditable = false
		);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;
		bool RenderLine(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cTexture *m_texture; // reference
		cMaterial m_mtrl;
		int m_rowCellCount;
		int m_colCellCount;
		float m_cellSizeW;
		float m_cellSizeH;
		int m_faceCount; // no line count
	};

}
