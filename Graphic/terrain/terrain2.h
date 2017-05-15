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

		void Update(cRenderer &renderer, const float deltaSeconds);
		void PreRender(cRenderer &renderer);
		void Render(cRenderer &renderer);
		void CullingTest(const cFrustum &frustum, const bool isModel=true);
		bool AddTile(cTile *model);
		bool RemoveTile(cTile *model);
		void Clear();


	public:
		vector<cTile*> m_tiles;		
	};

}
