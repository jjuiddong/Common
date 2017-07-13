//
// 2017-05-13, jjuiddong
// terrain 
// - lighting, model
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
		virtual bool Update(cRenderer &renderer, const float deltaSeconds) override;
		virtual void PreRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity
			, const int shadowMapIdx=0);
		virtual bool Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity, const int flags = 1) override;
		virtual void RenderOption(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity, const int option=0x1);
		virtual void RenderDebug(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		virtual void LostDevice();
		virtual void ResetDevice(cRenderer &renderer);
		virtual void Clear();

		void UpdateShader(cRenderer &renderer);
		void CullingTest(cRenderer &renderer, cCamera &camera, const bool isModel=true
			, const int shadowMapIdx = 0);
		void CullingTest(cRenderer &renderer, const cFrustum &frustum, const bool isModel = true
			, const int shadowMapIdx = 0);
		void CullingTestOnly(cRenderer &renderer, cCamera &camera, const bool isModel = true);

		bool AddTile(cTile *model);
		bool RemoveTile(cTile *model);
		//cTile* FindTile(const Str64 &name);
		//cTile* FindTile(const int id);
		//cNode2* FindModel(const int modelId);
		void SetDbgRendering(const bool isRender);
		void SetShadowRendering(const bool isRender);


	public:
		bool m_isShadow;
		int m_rows;
		int m_cols;
		vector<cTile*> m_tiles; // reference
		map<hashcode, cTile*> m_tilemap; // reference
		map<int, cTile*> m_tilemap2; // reference
		set<cShader*> m_shaders; // reference

		// ShadowMap
		enum { SHADOWMAP_COUNT=3 };
		cCamera m_lightCam[ SHADOWMAP_COUNT];
		cDbgFrustum m_frustum[ SHADOWMAP_COUNT];
		Matrix44 m_lightView[ SHADOWMAP_COUNT];
		Matrix44 m_lightProj[ SHADOWMAP_COUNT];
		Matrix44 m_lightTT[ SHADOWMAP_COUNT];
		cShadowMap m_shadowMap[ SHADOWMAP_COUNT];

		// Debug Display
		bool m_isShowDebug;
		cDbgArrow m_dbgLight;
		cLine m_dbgPlane; // Plane Normal Vector
		cDbgFrustum m_dbgLightFrustum[SHADOWMAP_COUNT];
		cDbgSphere m_dbgSphere;
	};

}
