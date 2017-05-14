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

		bool Create(cRenderer &renderer, sRectf &rect, const float y=0);
		void Update(cRenderer &renderer, const float deltaSeconds);
		void PreRender(cRenderer &renderer);
		void Render(cRenderer &renderer);
		bool AddModel(cModel2 *model);
		bool RemoveModel(cModel2 *model);
		void Clear();


	public:
		bool m_isShadow;
		cGrid3 m_ground;
		cLight m_light;
		Matrix44 m_tm;
		cBoundingBox m_boundingBox;
		vector<cModel2*> m_models;
		cShadowMap m_shadowMap;

		Matrix44 m_viewtoLightProj;
		cDbgBox m_dbgTile;
		cDbgArrow m_dbgLight;
	};

}
