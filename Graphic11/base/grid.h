//
// 2017-05-06, jjuiddong
// VertexFormat = Point + Color
//
#pragma once

namespace graphic
{
	class cRenderer;

	class cGrid : public cNode2
	{
	public:
		cGrid();
		virtual ~cGrid();

		void Create(cRenderer &renderer, const int rowCellCount, const int colCellCount, const float cellSize);
		//void Render(cRenderer &renderer);
		//void RenderLinelist(cRenderer &renderer);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;

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
