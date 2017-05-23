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
		virtual void PreRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		virtual void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		virtual void LostDevice();
		virtual void ResetDevice(cRenderer &renderer);
		virtual void Clear();

		void UpdateShader(cRenderer &renderer);
		void CullingTest(cRenderer &renderer, cCamera &camera, const bool isModel=true);
		bool AddTile(cTile *model);
		cTile* FindTile(const string &name);
		bool RemoveTile(cTile *model);
		void SetDbgRendering(const bool isRender);


	public:
		cLight m_light;
		cCamera m_lightCam;
		cDbgFrustum m_frustum;
		vector<cTile*> m_tiles;
		map<string, cTile*> m_tilemap; // reference

		// Shadow
		Matrix44 m_VPT; // ShadowMap Transform, = light view x light proj x uv transform
		Matrix44 m_LVP; // ShadowMap Transform, Light View Projection, = light view x light proj
		cSurface2 m_shadowSurf;
		cShadowMap m_shadowMap;

		// Debug Display
		bool m_isShowDebug;
		cDbgArrow m_dbgLight;
		cLine m_dbgPlane;
		cDbgFrustum m_dbgLightFrustum;
	};

}
