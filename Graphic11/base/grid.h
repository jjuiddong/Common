//
// 2017-05-06, jjuiddong
// Grid Class
//
#pragma once

namespace graphic
{

	class cRenderer;
	class cGrid : public cNode
	{
	public:
		cGrid();
		virtual ~cGrid();

		void Create(cRenderer &renderer, const int rowCellCount, const int colCellCount, const float cellSize
			, const int vertexType = (eVertexType::POSITION | eVertexType::DIFFUSE)
			, const cColor &color = cColor::WHITE
			, const char *textureFileName = g_defaultTexture
			, const Vector2 &uv0 = Vector2(0, 0)
			, const Vector2 &uv1 = Vector2(1, 1)
			, const float textureUVFactor = 1.f
			, const bool isEditable = false

		);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cTexture *m_texture; // reference
		cMaterial m_mtrl;
		bool m_isLineDrawing;
		int m_rowCellCount;
		int m_colCellCount;
		float m_cellSize;
		int m_vertexType; // compose eVertexType::Enum
		D3D_PRIMITIVE_TOPOLOGY m_primitiveType;
	};

}
