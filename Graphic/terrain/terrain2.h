//
// 2017-05-13, jjuiddong
// terrain 
// - lighting, model
// 
#pragma once


namespace graphic
{

	class cTile;
	class cTerrain2
	{
	public:
		cTerrain2();
		virtual ~cTerrain2();

		virtual bool Create(cRenderer &renderer, const sRectf &rect);
		virtual void Update(cRenderer &renderer, const float deltaSeconds);
		virtual void PreRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity
			, const int shadowMapIdx=0);
		virtual void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
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
		cTile* FindTile(const string &name);
		cModel2* FindModel(const int modelId);
		bool RemoveTile(cTile *model);
		void SetDbgRendering(const bool isRender);
		void SetShadowRendering(const bool isRender);


	public:
		bool m_isShadow;
		vector<cTile*> m_tiles;
		map<string, cTile*> m_tilemap; // reference

		// ShadowMap
		enum { SHADOWMAP_COUNT=3 };
		cCamera m_lightCam[ SHADOWMAP_COUNT];
		cDbgFrustum m_frustum[ SHADOWMAP_COUNT];
		Matrix44 m_lightView[ SHADOWMAP_COUNT];
		Matrix44 m_lightProj[ SHADOWMAP_COUNT];
		Matrix44 m_lightTT[ SHADOWMAP_COUNT];
		cShadowMap m_shadowMap[ SHADOWMAP_COUNT];

		//Matrix44 m_VPT; // ShadowMap Transform, = light view x light proj x uv transform
		//Matrix44 m_LVP; // ShadowMap Transform, Light View Projection, = light view x light proj
		//Matrix44 m_LV; // ShadowMap Transform, Light View Projection, = light view

		// Debug Display
		bool m_isShowDebug;
		cDbgArrow m_dbgLight;
		cLine m_dbgPlane; // Plane Normal Vector
		cDbgFrustum m_dbgLightFrustum[SHADOWMAP_COUNT];
	};

}
