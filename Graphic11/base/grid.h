//
// 2017-05-06, jjuiddong
// Grid Class
//
#pragma once

namespace graphic
{

	struct eGridType {
		enum Enum {
			POSITION = 1 << 1
			, NORMAL = 1 << 2
			, DIFFUSE = 1 << 3
			, TEXTURE = 1 << 4
		};
	};

	class cRenderer;
	class cGrid : public cNode2
	{
	public:
		cGrid();
		virtual ~cGrid();

		void Create(cRenderer &renderer, const int rowCellCount, const int colCellCount, const float cellSize
			, const int gridType = (eGridType::POSITION | eGridType::DIFFUSE)
			, const Vector2 &uv0 = Vector2(0, 0)
			, const Vector2 &uv1 = Vector2(1, 1)
			, const float textureUVFactor = 1.f
		);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		cTexture *m_texture; // reference
		int m_rowCellCount;
		int m_colCellCount;
		float m_cellSize;
		int m_gridType; // eGridType::Enum
		D3D_PRIMITIVE_TOPOLOGY m_primitiveType;
	};

}
