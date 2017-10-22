//
// 2017-09-27, jjuiddong
//
// Terrain Edit Cusor
// Upgrade Legacy Code
//
#pragma once


namespace graphic
{
	struct eGeoEditType {
		enum Enum { UP, DOWN, FLAT };
	};


	class cTerrainCursor : public cNode
	{
	public:
		cTerrainCursor();
		virtual ~cTerrainCursor(void);

		bool Create(cRenderer &renderer);
		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		void GeometryEdit(cRenderer &renderer, cTerrain2 &terrain, const eGeoEditType::Enum editType
			, const Vector3 &cursorPos, const float deltaSeconds, const bool isHilightTile = false);

		void UpdateCursor(cRenderer &renderer, cTerrain2 &terrain, const Vector3 &cursorPos);


	public:
		enum {CURSOR_VERTEX_COUNT = 64};

		Vector3 m_pos;
		float m_brushSpeed; // meter/sec, defalut: 20
		float m_innerRadius;
		float m_outerRadius;
		float m_innerAlpha;
		cVertexBuffer m_innerCircle;
		cVertexBuffer m_outerCircle;
		cTemporalBuffer m_cpyVtxBuff;
		cTemporalBuffer m_cpyIdxBuff;
	};

}
