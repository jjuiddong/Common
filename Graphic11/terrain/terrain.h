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
	class cTerrain : public cNode
	{
	public:
		cTerrain();
		virtual ~cTerrain();

		virtual bool Create(cRenderer &renderer, const sRectf &rect);
		virtual bool Create(cRenderer &renderer, const int rowCnt, const int colCnt
			, const float cellSizeW, const float cellSizeH
			, const int rowTileCnt, const int colTileCnt );

		virtual bool Render(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int flags = 1) override;

		void BuildCascadedShadowMap(cRenderer &renderer, INOUT cCascadedShadowMap &ccsm, const XMMATRIX &tm = XMIdentity);
		bool RenderCascadedShadowMap(cRenderer &renderer, cCascadedShadowMap &ccsm, const XMMATRIX &tm = XMIdentity, const int flags = 1);

		virtual void RenderOption(cRenderer &renderer, const XMMATRIX &tm = XMIdentity, const int option=0x1);
		virtual void RenderDebug(cRenderer &renderer, const XMMATRIX &tm = XMIdentity);
		virtual void Clear();

		void CullingTestOnly(cRenderer &renderer, cCamera &camera, const bool isModel = true
			, const XMMATRIX &tm = XMIdentity);
		bool AddTile(cTile *model);
		bool RemoveTile(cTile *model, const bool rmInstance = true);
		bool AddModel(cNode *model);
		bool UpdateModel(cNode *model);
		void SetShadowRendering(const bool isRender);

		// Heightmap
		float GetHeight(const float x, const float z);
		float GetHeightMapEntry(int row, int col);
		Vector3 GetHeightFromRay(const Ray &ray);
		void HeightmapNormalize(const Vector3 &cursorPos, const float radius);
		void HeightmapNormalize();
		void UpdateHeightmapToTile(cRenderer &renderer, cTile *tiles[], const int tileCount);

		void GetRect3D(cRenderer &renderer
			, const Vector3 &p0, const Vector3 &p1
			, OUT cRect3D &out
			, const int maxEdgeVertexCount = 32
			, const Vector3 &offset = Vector3(0, 0.15f, 0)
		);


	protected:
		cTile* GetNearestTile(const cNode *node);


	public:
		vector<cTile*> m_tiles; // reference
		map<hashcode, cTile*> m_tilemap; // reference (key = name hashcode)
		map<int, cTile*> m_tilemap2; // reference (key = id)

		// Terrain Edit
		vector<sVertexNorm> m_heightMap; // row tile * col tile * tile vertex count
										 // m_map[row][col];
		int m_cols;  // column cell count
		int m_rows;	 // row cell count
		Vector2 m_cellSize;
		int m_colVtx;  // column vertex count
		int m_rowVtx;	// row vertex count
		int m_tileCols; // row tile count
		int m_tileRows; // column tile count
		sRectf m_rect;
		float m_defaultHeight; // out of range terrain height
		float m_lerpAlphaFactor; // use lerp alpha factor
		cTemporalBuffer m_cpyVtxBuff;
		cTemporalBuffer m_cpyIdxBuff;

		// Debug Display
		bool m_isShowDebug;
	};

}
