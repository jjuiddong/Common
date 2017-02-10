#pragma once

namespace graphic
{
	class cRenderer;

	class cGrid
	{
	public:
		cGrid();
		virtual ~cGrid();

		void Create(cRenderer &renderer, const int rowCellCount, const int colCellCount, const float cellSize);
		void Render(cRenderer &renderer);
		void RenderLinelist(cRenderer &renderer);

		cVertexBuffer& GetVertexBuffer();
		cIndexBuffer& GetIndexBuffer();


	private:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		int m_rowCellCount;
		int m_colCellCount;
		float m_cellSize;
	};


	inline cVertexBuffer& cGrid::GetVertexBuffer() { return m_vtxBuff; }
	inline cIndexBuffer& cGrid::GetIndexBuffer() { return m_idxBuff; }
}
