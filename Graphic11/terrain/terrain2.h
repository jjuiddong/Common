//
// 2017-05-13, jjuiddong
// terrain 
// - lighting, model
//
// 2017-08-11
//	- Upgrade DX11
// 
#pragma once


namespace graphic
{

	class cTile;
	class cTerrain2 : public cNode2
	{
	public:
		cTerrain2();
		virtual ~cTerrain2();

		virtual bool Create(cRenderer &renderer, const sRectf &rect);
		virtual bool Create(cRenderer &renderer, const int rowCnt, const int colCnt, const float cellSize
			, const int rowTileCnt, const int colTileCnt );

		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;

		void BuildCascadedShadowMap(cRenderer &renderer, INOUT cCascadedShadowMap &ccsm, const XMMATRIX &tm = XMIdentity);
		bool RenderCascadedShadowMap(cRenderer &renderer, cCascadedShadowMap &ccsm, const XMMATRIX &tm = XMIdentity, const int flags = 1);

		virtual void RenderOption(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int option=0x1);
		virtual void RenderDebug(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);
		virtual void Clear();

		void CullingTestOnly(cRenderer &renderer, cCamera &camera, const bool isModel = true);
		bool AddTile(cTile *model);
		bool RemoveTile(cTile *model);
		void SetDbgRendering(const bool isRender);
		void SetShadowRendering(const bool isRender);

		// Heightmap
		float GetHeight(const float x, const float z);
		float GetHeightMapEntry(int row, int col);
		void HeightmapNormalize(const Vector3 &cursorPos, const float radius);
		void HeightmapNormalize();
		void UpdateHeightmapToTile(cRenderer &renderer, cTile *tiles[], const int tileCount);


	public:
		vector<cTile*> m_tiles; // reference
		map<hashcode, cTile*> m_tilemap; // reference (key = name hashcode)
		map<int, cTile*> m_tilemap2; // reference (key = id)

		// Debug Display
		bool m_isShowDebug;
		cDbgArrow m_dbgLightDir;
		cLine m_dbgPlane; // Plane Normal Vector

		// Terrain Edit
		vector<sVertexNorm> m_heightMap; // row tile * col tile * tile vertex count
										 // m_map[row][col];
		int m_cols;  // column cell count
		int m_rows;	 // row cell count
		float m_cellSize;
		int m_colVtx;  // column vertex count
		int m_rowVtx;	 // row vertex count
		int m_tileCols; // row tile count
		int m_tileRows; // column tile count
		cTemporalBuffer m_cpyVtxBuff;
		cTemporalBuffer m_cpyIdxBuff;
	};

}
