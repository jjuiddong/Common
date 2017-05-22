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

		bool Create(cRenderer &renderer, const sRectf &rect);
		void Update(cRenderer &renderer, const float deltaSeconds);
		void PreRender(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void Render(cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);
		void UpdateShader(cRenderer &renderer);
		void CullingTest(cRenderer &renderer, cCamera &camera, const bool isModel=true);
		bool AddTile(cTile *model);
		cTile* FindTile(const string &name);
		bool RemoveTile(cTile *model);
		void SetDbgRendering(const bool isRender);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();


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

		// Debug
		bool m_isShowShadowMap;
		cDbgArrow m_dbgLight;
		cLine m_dbgPlane;
		cDbgFrustum m_dbgLightFrustum;
	};

}
