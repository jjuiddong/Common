//
// 2017-05-13, jjuiddong
// terrain tile
//
#pragma once


namespace graphic
{

	class cTile
	{
	public:
		cTile();
		virtual ~cTile();

		bool Create(cRenderer &renderer
			, const string &name
			, const sRectf &rect
			, const float y=0, const float uvFactor=1.f
			, const Vector2 &uv0 = Vector2(0, 0), const Vector2 &uv1 = Vector2(1, 1)
			, const int dbgIdx = 2);
		void Update(cRenderer &renderer, const float deltaSeconds);
		void PreRender(cRenderer &renderer, cShadowMap &shadowMap);
		void Render(cRenderer &rendererm, cShadowMap *shadowMap=NULL);
		float CullingTest(const cFrustum &frustum, const bool isModel = true);
		bool AddModel(cModel2 *model);
		bool RemoveModel(cModel2 *model);
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();


	public:
		bool m_isShadow;
		bool m_isCulling; // Out of Frustum = true
		bool m_isDbgRender;
		string m_name;
		cGrid3 m_ground;
		//cLight m_light;
		Matrix44 m_tm;
		cBoundingBox m_boundingBox;
		vector<cModel2*> m_models;
		
		Matrix44 m_VPT; // ShadowMap Transform, = light view x light proj x uv transform
		Matrix44 m_LVP; // ShadowMap Transform, Light View Projection, = light view x light proj
		//cShadowMap m_shadowMap;

		cDbgBox m_dbgTile;
		//cDbgArrow m_dbgLight;
		//cDbgFrustum m_dbgLightFrustum;
		int m_dbgIdx; // Surface Rendering Index
	};

}
