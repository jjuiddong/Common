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
		void PreRender(cRenderer &renderer);
		void Render(cRenderer &renderer);
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
		vector<cTile*> m_tiles;
		map<string, cTile*> m_tilemap; // reference
		cLight m_light;
		cDbgFrustum m_frustum;
		cShadowMap m_shadowMap;

		cDbgArrow m_dbgLight;
		cLine m_dbgPlane;
		cDbgFrustum m_dbgLightFrustum;
	};

}
