//
// 2017-12-06, jjuiddong
// GridLine Class
//
#pragma once

namespace graphic
{
	using namespace common;

	class cRenderer;
	class cGridLine : public cNode
	{
	public:
		cGridLine();
		virtual ~cGridLine();

		void Create(cRenderer &renderer, const int rowCellCount, const int colCellCount
			, const float cellSizeW
			, const float cellSizeH
			, const int vertexType = (eVertexType::POSITION | eVertexType::COLOR)
			, const cColor &color1 = cColor(0.6f, 0.6f, 0.6f, 1.f)
			, const cColor &color2 = cColor(1.f, 1.f, 1.f, 1.f)
			, const bool isCell5 = true
			, const bool isAxis = true
		);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;


	public:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		int m_rowCellCount;
		int m_colCellCount;
		float m_cellSizeW;
		float m_cellSizeH;
		cColor m_lineColor;
		float m_offsetY; // default: 0.1f
	};

}
